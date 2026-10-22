// Palletino: 3-DOF teach-and-repeat arm firmware
// ESP32 + Waveshare Bus Servo Adapter (A) + 3x Feetech STS3215
//
// Open a serial monitor at 115200 baud (newline line ending) and type `help`.
//
// Commands
//   help                 list commands
//   scan                 ping servo IDs 0..20
//   status               positions, joint angles, voltage, temperature, load
//   pos                  one-line raw positions
//   stream on|off        print positions 10x per second (handy for finding limits)
//   torque on|off        hold the current pose / go limp so the arm can be dragged
//   home                 move to HOME_POS
//   goto <a> <b> <c>     move all joints to raw positions (clamped to soft limits)
//   set <joint> <raw>    move one joint (joint = 1..3)
//   teach                go limp and record while you move the arm by hand
//   stop                 end teach / playback (playback stops holding position)
//   play [loops]         replay the recording; loops 0 = forever (default 1)
//   save | load | clear  keep the recording in flash / bring it back / wipe RAM copy
//   info                 recording length
//   setid <old> <new>    change a servo ID (ONLY ONE servo on the bus!)
//   centre <id>          make that servo's current position read as 2048
//   reset                clear an E-stop after the button is released
//
// E-stop: contact #1 physically cuts the servo rail. Contact #2 tells this code,
// which aborts teach/playback and refuses motion until `reset`. After reset the
// servos come back limp, so nothing moves until you ask it to.

#include <Arduino.h>
#include "config.h"
#include "arm.h"
#include "recorder.h"

enum class Mode { Idle, Teach, Approach, Play, EStop };

static Arm      arm;
static Recorder rec;
static Mode     mode = Mode::Idle;

// teach / playback bookkeeping
static const uint32_t FRAME_MS = 1000 / RECORD_HZ;
static uint32_t nextTick       = 0;
static uint8_t  readFailures   = 0;
static uint16_t playIndex      = 0;
static uint32_t loopsWanted    = 1;   // 0 = forever
static uint32_t loopsDone      = 0;
static uint32_t approachStart  = 0;

// e-stop debouncing
static bool     estopTripped   = false;
static bool     estopRaw       = false;
static uint32_t estopChangedAt = 0;
static bool     releaseNoticed = false;

// misc
static bool     streaming      = false;
static uint32_t nextStream     = 0;
static char     line[96];
static uint8_t  lineLen        = 0;

static const char* modeName(Mode m) {
  switch (m) {
    case Mode::Idle:     return "idle";
    case Mode::Teach:    return "teach";
    case Mode::Approach: return "moving to start";
    case Mode::Play:     return "playback";
    case Mode::EStop:    return "E-STOP";
  }
  return "?";
}

// ---------------------------------------------------------------------------
// LED: slow blink idle, fast blink teach, solid during playback, flicker on E-stop
// ---------------------------------------------------------------------------
static void updateLed() {
  uint32_t period;
  switch (mode) {
    case Mode::Idle:  period = 1000; break;
    case Mode::Teach: period = 150;  break;
    case Mode::EStop: period = 60;   break;
    default:          digitalWrite(PIN_LED, HIGH); return;
  }
  digitalWrite(PIN_LED, (millis() / period) % 2);
}

// ---------------------------------------------------------------------------
// E-stop
// ---------------------------------------------------------------------------
static void enterEStop() {
  if (mode == Mode::Teach && !rec.empty()) {
    Serial.printf("Teach interrupted; kept %u frames (%.1f s).\n", rec.size(), rec.seconds());
  }
  mode = Mode::EStop;
  arm.forgetTorque();
  releaseNoticed = false;
  Serial.println("!! E-STOP: servo power cut. Release the button, then type `reset`.");
}

static void pollEStop() {
  bool raw = digitalRead(PIN_ESTOP_SENSE) == HIGH;   // HIGH = contact open = stop
  if (raw != estopRaw) { estopRaw = raw; estopChangedAt = millis(); }
  if (millis() - estopChangedAt < ESTOP_DEBOUNCE_MS) return;

  if (estopRaw && !estopTripped) {
    estopTripped = true;
    enterEStop();
  } else if (!estopRaw && estopTripped) {
    estopTripped = false;
  }
  if (mode == Mode::EStop && !estopTripped && !releaseNoticed) {
    releaseNoticed = true;
    Serial.println("E-stop released. Type `reset` to continue.");
  }
}

static bool motionAllowed() {
  if (mode == Mode::EStop) { Serial.println("Blocked: E-stop active. Release it and `reset`."); return false; }
  if (mode != Mode::Idle)  { Serial.printf("Busy (%s). Type `stop` first.\n", modeName(mode)); return false; }
  return true;
}

// ---------------------------------------------------------------------------
// Teach
// ---------------------------------------------------------------------------
static void startTeach() {
  if (!motionAllowed()) return;
  if (!arm.setTorque(false)) {
    Serial.println("Couldn't reach every servo; teach not started. Try `scan`.");
    return;
  }
  rec.clear();
  readFailures = 0;
  nextTick = millis();
  mode = Mode::Teach;
  Serial.printf("Teach: arm is limp, recording at %u Hz (max %.0f s). Move it, then `stop`.\n",
                RECORD_HZ, MAX_FRAMES / (float)RECORD_HZ);
}

static void stopTeach(const char* why) {
  mode = Mode::Idle;
  Serial.printf("%sRecorded %u frames (%.1f s). `play` to test, `save` to keep it.\n",
                why, rec.size(), rec.seconds());
}

static void tickTeach() {
  if ((int32_t)(millis() - nextTick) < 0) return;
  nextTick += FRAME_MS;

  int16_t now[NUM_JOINTS];
  if (!arm.readAll(now)) {
    if (++readFailures >= 10) stopTeach("Lost contact with a servo. ");
    return;
  }
  readFailures = 0;
  if (!rec.push(now)) stopTeach("Buffer full. ");
}

// ---------------------------------------------------------------------------
// Playback
// ---------------------------------------------------------------------------
static void beginApproach() {
  arm.moveAll(rec.at(0).pos, MOVE_SPEED, MOVE_ACC);
  approachStart = millis();
  mode = Mode::Approach;
}

static void startPlayback(uint32_t loops) {
  if (!motionAllowed()) return;
  if (rec.empty()) { Serial.println("Nothing recorded. `teach` or `load` first."); return; }
  loopsWanted = loops;
  loopsDone = 0;
  Serial.printf("Playing %u frames (%.1f s), %s. `stop` to end.\n", rec.size(), rec.seconds(),
                loops == 0 ? "looping forever" : (loops == 1 ? "once" : "multiple loops"));
  beginApproach();
}

static void stopPlayback(const char* why) {
  arm.setTorque(true);   // hold wherever it is now
  mode = Mode::Idle;
  Serial.printf("%sPlayback stopped after %lu loop(s).\n", why, (unsigned long)loopsDone);
}

static void tickApproach() {
  if ((int32_t)(millis() - nextTick) < 0) return;
  nextTick = millis() + 50;

  int16_t now[NUM_JOINTS];
  if (arm.readAll(now)) {
    bool arrived = true;
    for (uint8_t j = 0; j < NUM_JOINTS; j++) {
      if (abs(now[j] - Arm::clampJoint(j, rec.at(0).pos[j])) > ARRIVE_TOLERANCE) arrived = false;
    }
    if (arrived) {
      playIndex = 1;
      nextTick = millis();
      mode = Mode::Play;
      return;
    }
  }
  if (millis() - approachStart > APPROACH_TIMEOUT_MS) {
    stopPlayback("Couldn't reach the start pose (blocked, or outside soft limits?). ");
  }
}

static void tickPlay() {
  if ((int32_t)(millis() - nextTick) < 0) return;
  nextTick += FRAME_MS;

  if (playIndex >= rec.size()) {
    loopsDone++;
    if (loopsWanted != 0 && loopsDone >= loopsWanted) {
      mode = Mode::Idle;   // torque stays on, arm holds the last pose
      Serial.println("Playback done.");
    } else {
      beginApproach();
    }
    return;
  }

  // Speed each joint so it covers this frame's distance in one frame period.
  const Frame& prev = rec.at(playIndex - 1);
  const Frame& next = rec.at(playIndex);
  uint16_t speed[NUM_JOINTS];
  for (uint8_t j = 0; j < NUM_JOINTS; j++) {
    uint32_t s = (uint32_t)abs(next.pos[j] - prev.pos[j]) * RECORD_HZ * 5 / 4 + 20;
    speed[j] = s > PLAYBACK_MAX_SPEED ? PLAYBACK_MAX_SPEED : s;
  }
  arm.moveAll(next.pos, speed, PLAYBACK_ACC);
  playIndex++;
}

// ---------------------------------------------------------------------------
// Console
// ---------------------------------------------------------------------------
static void printPositions() {
  int16_t now[NUM_JOINTS];
  if (!arm.readAll(now)) { Serial.println("read failed"); return; }
  for (uint8_t j = 0; j < NUM_JOINTS; j++) {
    Serial.printf("%s=%d%s", JOINT_NAME[j], now[j], j + 1 < NUM_JOINTS ? "  " : "\n");
  }
}

static void printHelp() {
  Serial.println(
    "help | scan | status | pos | stream on|off | torque on|off | home\n"
    "goto <a> <b> <c> | set <joint 1-3> <raw> | teach | stop | play [loops, 0=forever]\n"
    "save | load | clear | info | setid <old> <new> | centre <id> | reset");
}

static bool parseInt(const char* s, long& out) {
  if (!s) return false;
  char* end;
  out = strtol(s, &end, 10);
  return end != s && *end == '\0';
}

static void handleCommand(char* cmdLine) {
  char* cmd = strtok(cmdLine, " \t");
  if (!cmd) return;
  char* a1 = strtok(nullptr, " \t");
  char* a2 = strtok(nullptr, " \t");
  char* a3 = strtok(nullptr, " \t");
  long v1, v2, v3;

  if (!strcmp(cmd, "help")) {
    printHelp();
  } else if (!strcmp(cmd, "scan")) {
    arm.scan(Serial);
  } else if (!strcmp(cmd, "status")) {
    Serial.printf("mode %s | e-stop %s | recording %u frames (%.1f s)\n", modeName(mode),
                  estopTripped ? "PRESSED" : "ok", rec.size(), rec.seconds());
    arm.printStatus(Serial);
  } else if (!strcmp(cmd, "pos")) {
    printPositions();
  } else if (!strcmp(cmd, "stream")) {
    streaming = a1 && !strcmp(a1, "on");
    Serial.printf("stream %s\n", streaming ? "on" : "off");
  } else if (!strcmp(cmd, "torque")) {
    if (!a1 || (strcmp(a1, "on") && strcmp(a1, "off"))) { Serial.println("usage: torque on|off"); return; }
    if (!motionAllowed()) return;
    bool on = !strcmp(a1, "on");
    Serial.println(arm.setTorque(on) ? (on ? "torque on (holding)" : "torque off (limp)")
                                     : "some servos didn't answer");
  } else if (!strcmp(cmd, "home")) {
    if (!motionAllowed()) return;
    arm.moveAll(HOME_POS, MOVE_SPEED, MOVE_ACC);
    Serial.println("homing");
  } else if (!strcmp(cmd, "goto")) {
    if (!parseInt(a1, v1) || !parseInt(a2, v2) || !parseInt(a3, v3)) { Serial.println("usage: goto <a> <b> <c>"); return; }
    if (!motionAllowed()) return;
    int16_t target[NUM_JOINTS] = {(int16_t)v1, (int16_t)v2, (int16_t)v3};
    arm.moveAll(target, MOVE_SPEED, MOVE_ACC);
    Serial.printf("goto %d %d %d (after limits: %d %d %d)\n", (int)v1, (int)v2, (int)v3,
                  Arm::clampJoint(0, v1), Arm::clampJoint(1, v2), Arm::clampJoint(2, v3));
  } else if (!strcmp(cmd, "set")) {
    if (!parseInt(a1, v1) || !parseInt(a2, v2) || v1 < 1 || v1 > NUM_JOINTS) { Serial.println("usage: set <joint 1-3> <raw>"); return; }
    if (!motionAllowed()) return;
    int16_t target[NUM_JOINTS];
    if (!arm.readAll(target)) { Serial.println("read failed"); return; }
    target[v1 - 1] = (int16_t)v2;
    arm.moveAll(target, MOVE_SPEED, MOVE_ACC);
    Serial.printf("%s -> %d\n", JOINT_NAME[v1 - 1], Arm::clampJoint(v1 - 1, v2));
  } else if (!strcmp(cmd, "teach")) {
    startTeach();
  } else if (!strcmp(cmd, "stop")) {
    if (mode == Mode::Teach) stopTeach("");
    else if (mode == Mode::Play || mode == Mode::Approach) stopPlayback("");
    else Serial.println("nothing running");
  } else if (!strcmp(cmd, "play")) {
    long loops = 1;
    if (a1 && (!parseInt(a1, loops) || loops < 0)) { Serial.println("usage: play [loops]"); return; }
    startPlayback((uint32_t)loops);
  } else if (!strcmp(cmd, "save")) {
    if (mode == Mode::Teach) { Serial.println("`stop` teach first"); return; }
    Serial.println(rec.save() ? "saved to flash" : "save failed");
  } else if (!strcmp(cmd, "load")) {
    if (mode != Mode::Idle) { Serial.println("only while idle"); return; }
    Serial.println(rec.load() ? "loaded" : "nothing saved (or saved with different settings)");
    Serial.printf("%u frames (%.1f s)\n", rec.size(), rec.seconds());
  } else if (!strcmp(cmd, "clear")) {
    if (mode != Mode::Idle) { Serial.println("only while idle"); return; }
    rec.clear();
    Serial.println("recording cleared from RAM (flash copy untouched)");
  } else if (!strcmp(cmd, "info")) {
    Serial.printf("%u frames, %.1f s at %u Hz\n", rec.size(), rec.seconds(), RECORD_HZ);
  } else if (!strcmp(cmd, "setid")) {
    if (!parseInt(a1, v1) || !parseInt(a2, v2) || v2 < 0 || v2 > 253) { Serial.println("usage: setid <old> <new>"); return; }
    if (!motionAllowed()) return;
    Serial.println(arm.changeId(v1, v2) ? "ID changed" : "failed (is exactly one servo connected?)");
  } else if (!strcmp(cmd, "centre") || !strcmp(cmd, "center")) {
    if (!parseInt(a1, v1)) { Serial.println("usage: centre <id>"); return; }
    if (!motionAllowed()) return;
    Serial.println(arm.centreHere(v1) ? "current position is now 2048" : "failed");
  } else if (!strcmp(cmd, "reset")) {
    if (mode != Mode::EStop) { Serial.println("no E-stop to clear"); return; }
    if (estopTripped)        { Serial.println("E-stop still pressed"); return; }
    delay(200);   // let the servo rail come back up
    arm.setTorque(false);
    mode = Mode::Idle;
    Serial.println("E-stop cleared. Servos are limp; `torque on` or `home` when ready.");
  } else {
    Serial.printf("unknown command '%s' (try `help`)\n", cmd);
  }
}

static void pollConsole() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      line[lineLen] = '\0';
      handleCommand(line);
      lineLen = 0;
    } else if (lineLen < sizeof(line) - 1) {
      line[lineLen++] = c;
    }
  }
}

// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_ESTOP_SENSE, INPUT_PULLUP);
  delay(300);

  Serial.println("\nPalletino firmware. Type `help`.");
  arm.begin();
  if (!rec.begin()) Serial.println("warning: flash filesystem unavailable, save/load disabled");
  if (rec.load()) Serial.printf("Loaded saved motion: %u frames (%.1f s)\n", rec.size(), rec.seconds());

  estopRaw = estopTripped = digitalRead(PIN_ESTOP_SENSE) == HIGH;
  if (estopTripped) enterEStop();
  else arm.scan(Serial);
}

void loop() {
  pollEStop();
  pollConsole();

  switch (mode) {
    case Mode::Teach:    tickTeach();    break;
    case Mode::Approach: tickApproach(); break;
    case Mode::Play:     tickPlay();     break;
    default: break;
  }

  if (streaming && mode != Mode::EStop && millis() >= nextStream) {
    nextStream = millis() + 100;
    printPositions();
  }
  updateLed();
}
