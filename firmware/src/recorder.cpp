#include "recorder.h"
#include <LittleFS.h>

// File layout: "PLT1" | u16 joints | u16 rate | u16 count | frames...
static const char MAGIC[4] = {'P', 'L', 'T', '1'};

bool Recorder::begin() {
  fsOk_ = LittleFS.begin(true);
  return fsOk_;
}

bool Recorder::push(const int16_t pos[NUM_JOINTS]) {
  if (full()) return false;
  memcpy(frames_[count_].pos, pos, sizeof(Frame));
  count_++;
  return true;
}

bool Recorder::save(const char* path) const {
  if (!fsOk_) return false;
  File f = LittleFS.open(path, "w");
  if (!f) return false;
  uint16_t hdr[3] = {NUM_JOINTS, RECORD_HZ, count_};
  bool ok = f.write((const uint8_t*)MAGIC, 4) == 4 &&
            f.write((const uint8_t*)hdr, sizeof(hdr)) == sizeof(hdr) &&
            f.write((const uint8_t*)frames_, count_ * sizeof(Frame)) == count_ * sizeof(Frame);
  f.close();
  return ok;
}

bool Recorder::load(const char* path) {
  if (!fsOk_ || !LittleFS.exists(path)) return false;
  File f = LittleFS.open(path, "r");
  if (!f) return false;
  char magic[4];
  uint16_t hdr[3];
  bool ok = f.read((uint8_t*)magic, 4) == 4 && memcmp(magic, MAGIC, 4) == 0 &&
            f.read((uint8_t*)hdr, sizeof(hdr)) == sizeof(hdr) &&
            hdr[0] == NUM_JOINTS && hdr[1] == RECORD_HZ && hdr[2] <= MAX_FRAMES;
  if (ok) {
    size_t bytes = hdr[2] * sizeof(Frame);
    ok = f.read((uint8_t*)frames_, bytes) == bytes;
    count_ = ok ? hdr[2] : 0;
  }
  f.close();
  return ok;
}
