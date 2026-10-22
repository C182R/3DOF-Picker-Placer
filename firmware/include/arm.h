// Thin wrapper around the Feetech STS bus: reads, writes, torque, and limits.
#pragma once
#include <Arduino.h>
#include <SCServo.h>
#include "config.h"

class Arm {
public:
  void begin();

  // Ping IDs 0..maxId and print what answers. Returns how many responded.
  uint8_t scan(Print& out, uint8_t maxId = 20);

  // Read every joint. Returns false if any servo failed to answer.
  bool readAll(int16_t pos[NUM_JOINTS]);

  // Torque on holds the arm where it currently is (no jump); off lets you drag it.
  bool setTorque(bool on);
  bool torqueOn() const { return torque_; }
  void forgetTorque() { torque_ = false; }   // after the servo rail is cut

  // Move every joint to pos (clamped to soft limits). One packet for all joints.
  void moveAll(const int16_t pos[NUM_JOINTS], const uint16_t speed[NUM_JOINTS], uint8_t acc);
  void moveAll(const int16_t pos[NUM_JOINTS], uint16_t speed, uint8_t acc);

  // Print positions, angles, voltage, temperature and load for each joint.
  void printStatus(Print& out);

  // EEPROM helpers for first-time setup (one servo on the bus at a time!).
  bool changeId(uint8_t oldId, uint8_t newId);
  bool centreHere(uint8_t id);   // makes the current position read as 2048

  static int16_t clampJoint(uint8_t j, int32_t raw);
  static float   toJointDeg(uint8_t j, int16_t raw);

private:
  SMS_STS bus_;
  bool    torque_ = false;
};
