#include "pitches.h"
  
static const uint16_t init_melody[]            = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
static const uint8_t init_note_durations[]     = {4, 8, 8, 4, 4, 4, 4, 4};
static const uint8_t init_melody_duration      = 8;

static const uint16_t gain_10_melody[]         = {NOTE_C1, NOTE_G2, NOTE_G4, NOTE_A4};
static const uint8_t gain_10_note_durations[]  = {4, 8, 8, 4};
static const uint8_t gain_10_melody_duration   = 4;

static const uint16_t gain_100_melody[]        = {NOTE_B2, NOTE_F1, NOTE_E5, NOTE_A1};
static const uint8_t gain_100_note_durations[] = {4, 8, 8, 4};
static const uint8_t gain_100_melody_duration  = 4;