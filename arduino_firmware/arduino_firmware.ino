#define SCLK_PIN 6
#define RCLK_PIN 5
#define DIO_PIN 4

#define VIBRATION_PIN 2
#define TOUCH_PIN 3
#define SOUND_PIN A2

#define RED_LIGHT_PIN A5
#define GREEN_LIGHT_PIN A3
#define BLUE_LIGHT_PIN A4

#define HTTP_POST_SEND_TIME_MS 500
#define DISP_AWAKE_TIME_MS 3
#define VIBRATION_CHECK_TIME_MS 50
#define TOUCH_CHECK_TIME_MS 125
#define LIGHT_TIME_MS 180

#define SOUND_TIME_MS LIGHT_TIME_MS
#define SOUND_TOUCH_FREQ_HZ 800
#define SOUND_VIBR_FREQ_HZ 2500

#include <TM74HC595Display.h>
#include <SPI.h>
#include <Ethernet.h>

enum MELODY_TYPE {
  INIT_MELODY,
  GAIN_10_MELODY,
  GAIN_100_MELODY
};

TM74HC595Display disp(SCLK_PIN, RCLK_PIN, DIO_PIN);
EthernetClient client;

void setup() {
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  Ethernet.begin(mac);
  delay(2000);

  if (client.connect("afternoon-ravine-88100.herokuapp.com", 80)) {      
    client.println("POST /update HTTP/1.1");
    client.println("Host: afternoon-ravine-88100.herokuapp.com");
    client.println("Content-Type: application/x-www-form-urlencoded");      
    client.println("Content-Length: 46");
    client.println();  
    client.println("touches=0&vibrs=0&total=0&first_connected=true");
    client.println("Connection: close");
    client.println();
  }
  
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(TOUCH_PIN, INPUT);

  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(GREEN_LIGHT_PIN, OUTPUT);
  pinMode(BLUE_LIGHT_PIN, OUTPUT);
  pinMode(SOUND_PIN, OUTPUT);

  disp.clear();
  play_melody(INIT_MELODY);
  digitalWrite(RED_LIGHT_PIN, HIGH);
  disp.digit4(0);
}

void loop() {
  static uint16_t num_touches = 0;
  static uint16_t num_vibrs = 0;
  static uint16_t num_detected = 0;

  static uint64_t http_send_timer = 0;
  static uint64_t vibr_check_timer = 0;
  static uint64_t touch_check_timer = 0;
  static uint64_t light_timer = 0;
  static uint64_t disp_isr_timer = 0;

  static boolean is_vibr = false;
  static boolean is_touch = false;
  static boolean need_send = false;

  if (millis() - light_timer >= LIGHT_TIME_MS) {
    digitalWrite(GREEN_LIGHT_PIN, LOW);
    digitalWrite(BLUE_LIGHT_PIN, LOW);
    digitalWrite(RED_LIGHT_PIN, HIGH);

    light_timer = millis();
  }
  
  if (millis() - vibr_check_timer >= VIBRATION_CHECK_TIME_MS) {
    if (digitalRead(VIBRATION_PIN) == HIGH) {
      if (!is_vibr) {
        is_vibr = true;
        need_send = true;
        num_vibrs++;
        num_detected = num_vibrs + num_touches;
        disp.digit4(num_detected);
      
        digitalWrite(RED_LIGHT_PIN, LOW);
        digitalWrite(BLUE_LIGHT_PIN, LOW);
        digitalWrite(GREEN_LIGHT_PIN, HIGH);

        if (num_detected % 10 == 0 && num_detected != 0 && num_detected % 100 != 0) {
          play_melody(GAIN_10_MELODY);
        }
        else if (num_detected % 100 == 0 && num_detected != 0) {
          play_melody(GAIN_100_MELODY);
        }
        else {
          tone(SOUND_PIN, SOUND_VIBR_FREQ_HZ, SOUND_TIME_MS);
        }

        light_timer = millis();
      }
    }
    else if (is_vibr){
      is_vibr = false;
    }
    
    vibr_check_timer = millis();
  }

  if (millis() - touch_check_timer >= TOUCH_CHECK_TIME_MS) {
    if (digitalRead(TOUCH_PIN) == HIGH) {
      if (!is_touch) {
        is_touch = true;
        need_send = true;
        num_touches++;
        num_detected = num_vibrs + num_touches;
        disp.digit4(num_detected);
      
        digitalWrite(RED_LIGHT_PIN, LOW);
        digitalWrite(GREEN_LIGHT_PIN, LOW);
        digitalWrite(BLUE_LIGHT_PIN, HIGH);
      
        if (num_detected % 10 == 0 && num_detected != 0 && num_detected % 100 != 0) {
          play_melody(GAIN_10_MELODY);
        }
        else if (num_detected % 100 == 0 && num_detected != 0) {
          play_melody(GAIN_100_MELODY);
        }
        else {
          tone(SOUND_PIN, SOUND_TOUCH_FREQ_HZ, SOUND_TIME_MS);
        }

        light_timer = millis();
      }
    }
    else if (is_touch){
      is_touch = false;
    }
    
    touch_check_timer = millis();
  }
  
  if (millis() - disp_isr_timer >= DISP_AWAKE_TIME_MS) {
    disp.timerIsr();       
    disp_isr_timer = millis();
  }

  if (millis() - http_send_timer >= HTTP_POST_SEND_TIME_MS && need_send) {
    if (client.connect("afternoon-ravine-88100.herokuapp.com", 80)) {
      uint16_t content_length = strlen("touches=&vibrs=&total=&first_connected=false");
      
      client.println("POST /update HTTP/1.1");
      client.println("Host: afternoon-ravine-88100.herokuapp.com");
      client.println("Content-Type: application/x-www-form-urlencoded");
      
      client.print("Content-Length: ");
      content_length += count_digits_uint16_t(num_touches) + count_digits_uint16_t(num_vibrs) + count_digits_uint16_t(num_detected);
      client.println(content_length);
      client.println();
      
      client.print("touches=");
      client.print(num_touches);
      client.print("&vibrs=");
      client.print(num_vibrs);
      client.print("&total=");
      client.print(num_detected);
      client.println("&first_connected=false");
      
      client.println("Connection: close");
      client.println();
    }

    need_send = false;
    http_send_timer = millis();
  }
}

void play_melody(MELODY_TYPE melody_type) {
  #include "pitches.h"
  
  static const uint16_t init_melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
  static const uint8_t init_note_durations[] = {4, 8, 8, 4, 4, 4, 4, 4};
  static const uint8_t init_melody_duration = 8;

  static const uint16_t gain_10_melody[] = {NOTE_C1, NOTE_G2, NOTE_G4, NOTE_A4};
  static const uint8_t gain_10_note_durations[] = {4, 8, 8, 4};
  static const uint8_t gain_10_melody_duration = 4;

  static const uint16_t gain_100_melody[] = {NOTE_B2, NOTE_F1, NOTE_E5, NOTE_A1};
  static const uint8_t gain_100_note_durations[] = {4, 8, 8, 4};
  static const uint8_t gain_100_melody_duration = 4;
      
  switch(melody_type) {
    case INIT_MELODY:
      play_one_melody(init_melody, init_note_durations, init_melody_duration);
      break;
      
    case GAIN_10_MELODY:
      play_one_melody(gain_10_melody, gain_10_note_durations, gain_10_melody_duration);
      break;

    case GAIN_100_MELODY:
      play_one_melody(gain_100_melody, gain_100_note_durations, gain_100_melody_duration);
      break;
  }
}

void play_one_melody(const uint16_t melody[], const uint8_t note_durations[], const uint8_t num_notes) {
  for (uint8_t current_note = 0; current_note < num_notes; current_note++) {
    uint16_t note_duration = 1000 / note_durations[current_note];
    uint16_t pause_between_notes = note_duration * 1.30;
      
    tone(SOUND_PIN, melody[current_note], note_duration);
    delay(pause_between_notes);
    noTone(SOUND_PIN);
  } 
}

uint8_t count_digits_uint16_t(uint16_t val) {
  if (val > 9999) {
    return 5;
  }
  else if (val > 999) {
    return 4;
  }
  else if (val > 99) {
    return 3;
  }
  else if (val > 9) {
    return 2;
  }
  else {
    return 1;
  }
}
