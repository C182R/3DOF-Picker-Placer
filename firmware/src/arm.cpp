#include "arm.h"

void Arm::begin() {
  Serial1.begin(BUS_BAUD, SERIAL_8N1, PIN_BUS_RX, PIN_BUS_TX);
  bus_.pSerial   = &Serial1;
  bus_.IOTimeOut = BUS_TIMEOUT_MS;
}

uint8_t Arm::scan(Print& out, uint8_t maxId) {
  uint8_t found = 0;
  out.printf("Scanning IDs 0..%u at %lu baud\n", maxId, (unsigned long)BUS_BAUD);
  for (uint8_t id = 0; id <= maxId; id++) {
    if (bus_.Ping(id) == id) {
      int pos = bus_.ReadPos(id);
      out.printf("  ID %3u  pos %4d\n", id, pos);
      found++;
    }
  }
  if (!found) {
    out.println("  nothing answered. Check: servo power on, jumper on A, TX/RX (try swapping), baud.");
  }
  return found;
}

bool Arm::readAll(int16_t pos[NUM_JOINTS]) {
  bool ok = true;
  for (uint8_t j = 0; j < NUM_JOINTS; j++) {
    int p = bus_.ReadPos(JOINT_ID[j]);
    if (p < 0) { ok = false; continue; }
    pos[j] = (int16_t)p;
  }
  return ok;
}

bool Arm::setTorque(bool on) {
  if (on) {
    // Hold the current pose: set goal = present position before torque engages,
    // otherwise the servo snaps to whatever stale goal is in its registers.
    int16_t now[NUM_JOINTS];
    if (!readAll(now)) return false;
    moveAll(now, MOVE_SPEED, MOVE_ACC);
  }
  bool ok = true;
  for (uint8_t j = 0; j < NUM_JOINTS; j++) {
    if (bus_.EnableTorque(JOINT_ID[j], on ? 1 : 0) < 0) ok = false;
  }
  torque_ = on && ok;
  return ok;
}

void Arm::moveAll(const int16_t pos[NUM_JOINTS], const uint16_t speed[NUM_JOINTS], uint8_t acc) {
  // SyncWritePosEx modifies its arrays, so hand it copies.
  uint8_t  ids[NUM_JOINTS];
  int16_t  p[NUM_JOINTS];
  uint16_t s[NUM_JOINTS];
  uint8_t  a[NUM_JOINTS];
  for (uint8_t j = 0; j < NUM_JOINTS; j++) {
    ids[j] = JOINT_ID[j];
    p[j]   = clampJoint(j, pos[j]);
    s[j]   = speed[j];
    a[j]   = acc;
  }
  bus_.SyncWritePosEx(ids, NUM_JOINTS, p, s, a);
  torque_ = true;   // a position command engages torque on STS servos
}

void Arm::moveAll(const int16_t pos[NUM_JOINTS], uint16_t speed, uint8_t acc) {
  uint16_t s[NUM_JOINTS];
  for (uint8_t j = 0; j < NUM_JOINTS; j++) s[j] = speed;
  moveAll(pos, s, acc);
}

void Arm::printStatus(Print& out) {
  out.printf("torque %s\n", torque_ ? "ON" : "off");
  out.println("joint     id   raw    deg    volt  temp  load");
  for (uint8_t j = 0; j < NUM_JOINTS; j++) {
    uint8_t id = JOINT_ID[j];
    if (bus_.FeedBack(id) < 0) {
      out.printf("%-9s %2u   -- no response --\n", JOINT_NAME[j], id);
      continue;
    }
    int raw = bus_.ReadPos(-1);
    out.printf("%-9s %2u  %4d  %6.1f  %5.1f  %3dC  %4d\n",
               JOINT_NAME[j], id, raw, toJointDeg(j, raw),
               bus_.ReadVoltage(-1) / 10.0f, bus_.ReadTemper(-1), bus_.ReadLoad(-1));
  }
}

bool Arm::changeId(uint8_t oldId, uint8_t newId) {
  if (bus_.Ping(oldId) != oldId) return false;
  bus_.unLockEprom(oldId);
  bus_.writeByte(oldId, SMS_STS_ID, newId);
  bus_.LockEprom(newId);
  delay(20);
  return bus_.Ping(newId) == newId;
}

bool Arm::centreHere(uint8_t id) {
  return bus_.CalibrationOfs(id) >= 0;
}

int16_t Arm::clampJoint(uint8_t j, int32_t raw) {
  if (raw < JOINT_MIN[j]) return JOINT_MIN[j];
  if (raw > JOINT_MAX[j]) return JOINT_MAX[j];
  return (int16_t)raw;
}

float Arm::toJointDeg(uint8_t j, int16_t raw) {
  return (raw - 2048) * (360.0f / 4096.0f) / JOINT_RATIO[j];
}
