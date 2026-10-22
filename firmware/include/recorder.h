// Stores a taught motion in RAM and saves/loads it to flash (LittleFS).
#pragma once
#include <Arduino.h>
#include "config.h"

struct Frame {
  int16_t pos[NUM_JOINTS];
};

class Recorder {
public:
  bool begin();                      // mounts LittleFS (formats it the first time)

  void     clear() { count_ = 0; }
  bool     push(const int16_t pos[NUM_JOINTS]);   // false when the buffer is full
  uint16_t size() const { return count_; }
  bool     empty() const { return count_ == 0; }
  bool     full() const { return count_ >= MAX_FRAMES; }
  const Frame& at(uint16_t i) const { return frames_[i]; }
  float    seconds() const { return count_ / (float)RECORD_HZ; }

  bool save(const char* path = RECORDING_PATH) const;
  bool load(const char* path = RECORDING_PATH);

private:
  Frame    frames_[MAX_FRAMES];
  uint16_t count_ = 0;
  bool     fsOk_  = false;
};
