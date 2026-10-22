// Palletino firmware: all wiring and tuning values live here.
// Change these to match your arm; nothing else should need editing for a basic build.
#pragma once
#include <Arduino.h>

// ---------------------------------------------------------------------------
// Servo bus: ESP32 <-> Waveshare Bus Servo Adapter (A)
// Jumper must be on position A (UART control).
// Waveshare labels the header from the MCU's side, so wire TX->TX and RX->RX.
// If `scan` finds nothing, swap the two data wires before anything else.
// ---------------------------------------------------------------------------
constexpr int      PIN_BUS_TX   = 17;       // ESP32 GPIO17 -> adapter "TX"
constexpr int      PIN_BUS_RX   = 16;       // ESP32 GPIO16 <- adapter "RX"
constexpr uint32_t BUS_BAUD     = 1000000;  // STS3215 factory default
constexpr uint32_t BUS_TIMEOUT_MS = 5;      // per-read timeout (library default is 100)

// ---------------------------------------------------------------------------
// Safety and status IO
// E-stop contact #2 (NC) goes between PIN_ESTOP_SENSE and GND.
// Closed (normal) = LOW. Pressed OR wire broken = HIGH via pull-up = stop.
// ---------------------------------------------------------------------------
constexpr int      PIN_ESTOP_SENSE  = 27;
constexpr uint32_t ESTOP_DEBOUNCE_MS = 20;
constexpr int      PIN_LED          = 2;    // on-board LED on most ESP32 DevKits

// ---------------------------------------------------------------------------
// Joints
// Raw servo units: 0..4095 per servo revolution, 2048 = centre.
// ---------------------------------------------------------------------------
constexpr uint8_t NUM_JOINTS = 3;
constexpr uint8_t JOINT_ID[NUM_JOINTS]          = {1, 2, 3};
static const char* const JOINT_NAME[NUM_JOINTS] = {"waist", "shoulder", "elbow"};

// Servo turns per joint turn (waist runs through the 2:1 herringbone).
// Only used to print joint angles; motion is commanded in raw servo units.
constexpr float JOINT_RATIO[NUM_JOINTS] = {2.0f, 1.0f, 1.0f};

// Soft limits in raw servo units. Every commanded position is clamped to these.
// Defaults are the full servo range: measure your arm with `status` and tighten them.
constexpr int16_t JOINT_MIN[NUM_JOINTS] = {0,    0,    0};
constexpr int16_t JOINT_MAX[NUM_JOINTS] = {4095, 4095, 4095};

// Pose used by the `home` command.
constexpr int16_t HOME_POS[NUM_JOINTS] = {2048, 2048, 2048};

// ---------------------------------------------------------------------------
// Motion tuning
// Speed is in servo steps/s, acceleration in units of 100 steps/s^2 (0 = no ramp).
// ---------------------------------------------------------------------------
constexpr uint16_t MOVE_SPEED         = 800;   // home / goto / approach to first frame
constexpr uint8_t  MOVE_ACC           = 30;
constexpr uint16_t PLAYBACK_MAX_SPEED = 3000;  // STS3215 tops out around 3000 steps/s at 12 V
constexpr uint8_t  PLAYBACK_ACC       = 0;
constexpr int16_t  ARRIVE_TOLERANCE   = 15;    // steps; "close enough" to the first frame
constexpr uint32_t APPROACH_TIMEOUT_MS = 6000;

// ---------------------------------------------------------------------------
// Teach and repeat
// ---------------------------------------------------------------------------
constexpr uint16_t RECORD_HZ  = 25;                // samples per second
constexpr uint16_t MAX_FRAMES = RECORD_HZ * 120;   // 2 minutes of motion (~18 KB RAM)
static const char* const RECORDING_PATH = "/motion.bin";
