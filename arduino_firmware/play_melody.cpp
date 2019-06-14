#include <Arduino.h>

#include "play_melody.h"

void play_melody(MELODY_TYPE melody_type, const uint8_t sound_pin) {
  #include "melodies.h"
      
  switch(melody_type) {
    case INIT_MELODY:
      play_one_melody(init_melody, init_note_durations, init_melody_duration, sound_pin);
      break;
      
    case GAIN_10_MELODY:
      play_one_melody(gain_10_melody, gain_10_note_durations, gain_10_melody_duration, sound_pin);
      break;

    case GAIN_100_MELODY:
      play_one_melody(gain_100_melody, gain_100_note_durations, gain_100_melody_duration, sound_pin);
      break;
  }
}

void play_one_melody(const uint16_t melody[], const uint8_t note_durations[], const uint8_t num_notes, const uint8_t sound_pin) {
  for (uint8_t current_note = 0; current_note < num_notes; current_note++) {
    uint16_t note_duration = 1000 / note_durations[current_note];
    uint16_t pause_between_notes = note_duration * 1.20;
      
    tone(sound_pin, melody[current_note], note_duration);
    delay(pause_between_notes);
    noTone(sound_pin);
  }    
}
