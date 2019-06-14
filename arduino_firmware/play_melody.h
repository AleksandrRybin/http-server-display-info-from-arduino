#pragma once

#include <Arduino.h>

// типы мелодий которые могут играть
enum MELODY_TYPE {
  INIT_MELODY,     // начальная мелодия при включении
  GAIN_10_MELODY,  // каждые 10 очков
  GAIN_100_MELODY  // каждые 100 очков
};

void play_melody(MELODY_TYPE melody_type, const uint8_t sound_pin);

void play_one_melody(const uint16_t melody[], const uint8_t note_durations[], const uint8_t num_notes, const uint8_t sound_pin);
