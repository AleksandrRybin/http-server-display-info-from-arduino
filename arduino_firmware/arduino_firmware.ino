#define SCLK_PIN 6
#define RCLK_PIN 5
#define DIO_PIN 4

#define VIBRATION_PIN 2
#define TOUCH_PIN 3
#define SOUND_PIN A2

#define RED_LIGHT_PIN A5
#define GREEN_LIGHT_PIN A3
#define BLUE_LIGHT_PIN A4

#define HTTP_POST_SEND_TIME_MS 335
#define DISP_AWAKE_TIME_MS 3
#define VIBRATION_CHECK_TIME_MS 50
#define TOUCH_CHECK_TIME_MS 125
#define LIGHT_TIME_MS 160

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
  
  http_send_post(0, 0, 0, true);
  
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

  check_light(false);
  check_display();

  boolean touch = check_touch();
  if (touch) {
    num_touches++;
      
    digitalWrite(RED_LIGHT_PIN, LOW);
    digitalWrite(GREEN_LIGHT_PIN, LOW);
    digitalWrite(BLUE_LIGHT_PIN, HIGH);

    tone(SOUND_PIN, SOUND_TOUCH_FREQ_HZ, SOUND_TIME_MS);
  }

  boolean vibr = check_vibr();
  if (vibr) {
    num_vibrs++;
      
    digitalWrite(RED_LIGHT_PIN, LOW);
    digitalWrite(BLUE_LIGHT_PIN, LOW);
    digitalWrite(GREEN_LIGHT_PIN, HIGH);
    
    tone(SOUND_PIN, SOUND_VIBR_FREQ_HZ, SOUND_TIME_MS);
  }

  if (touch || vibr) {
    num_detected = num_vibrs + num_touches;
    disp.digit4(num_detected);
    http_send_post(num_touches, num_vibrs, num_detected, false);
    check_light(true);
    
    if (num_detected % 10 == 0 && num_detected != 0 && num_detected % 100 != 0) {
      play_melody(GAIN_10_MELODY);
    }
    else if (num_detected % 100 == 0 && num_detected != 0) {
      play_melody(GAIN_100_MELODY);
    }
  }
}

void http_send_post(uint16_t num_touches, uint16_t num_vibrs, uint16_t num_detected, boolean first_connected) {
  static uint64_t http_send_timer = 0;

  if (millis() - http_send_timer >= HTTP_POST_SEND_TIME_MS) {
    if (Ethernet.hardwareStatus() != EthernetNoHardware && Ethernet.linkStatus() != LinkOFF) {
      if (client.connect("afternoon-ravine-88100.herokuapp.com", 80)) {
        uint16_t content_length = strlen("touches=&vibrs=&total=&first_connected=");
      
        client.println("POST /update HTTP/1.1");
        client.println("Host: afternoon-ravine-88100.herokuapp.com");
        client.println("Content-Type: application/x-www-form-urlencoded");
        
        client.print("Content-Length: ");
        content_length += count_digits_uint(num_touches) + count_digits_uint(num_vibrs) + count_digits_uint(num_detected);
        content_length += first_connected ? strlen("true") : strlen("false");
        
        client.println(content_length);
        client.println();
      
        client.print("touches=");
        client.print(num_touches);
        client.print("&vibrs=");
        client.print(num_vibrs);
        client.print("&total=");
        client.print(num_detected);
        
        client.print("&first_connected=");
        if (first_connected) {
          client.println("true");
        }
        else {
          client.println("false");
        }
      
        client.println("Connection: close");
        client.println();
      }
    }
    
    http_send_timer = millis();
  }
}

boolean check_vibr() {
  static uint64_t vibr_check_timer = 0;
  static boolean is_vibr = false;

  if (millis() - vibr_check_timer >= VIBRATION_CHECK_TIME_MS) {
    if (digitalRead(VIBRATION_PIN) == HIGH) {
      if (!is_vibr) {
        is_vibr = true;
        vibr_check_timer = millis();
        
        return true;
      }
    }
    else if (is_vibr){
      is_vibr = false;
    }
    
    vibr_check_timer = millis();
  }

  return false;
}

boolean check_touch() {
  static uint64_t touch_check_timer = 0;
  static boolean is_touch = false;

  if (millis() - touch_check_timer >= TOUCH_CHECK_TIME_MS) {
    if (digitalRead(TOUCH_PIN) == HIGH) {
      if (!is_touch) {
        is_touch = true;
        touch_check_timer = millis();

        return true;
      }
    }
    else if (is_touch){
      is_touch = false;
    }
    
    touch_check_timer = millis();
  }

  return false;
}

void check_light(boolean reinstall_timer) {
  static uint64_t light_timer = 0;

  if (reinstall_timer) {
    light_timer = millis();
  }
  
  if (millis() - light_timer >= LIGHT_TIME_MS) {
    digitalWrite(GREEN_LIGHT_PIN, LOW);
    digitalWrite(BLUE_LIGHT_PIN, LOW);
    digitalWrite(RED_LIGHT_PIN, HIGH);

    light_timer = millis();
  }
}

void check_display() {
  static uint64_t disp_isr_timer = 0;
  
  if (millis() - disp_isr_timer >= DISP_AWAKE_TIME_MS) {
    disp.timerIsr();       
    disp_isr_timer = millis();
  }
}

void play_melody(MELODY_TYPE melody_type) {
  #include "melody.h"
      
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
    uint16_t pause_between_notes = note_duration * 1.20;
      
    tone(SOUND_PIN, melody[current_note], note_duration);
    delay(pause_between_notes);
    noTone(SOUND_PIN);
  } 
}

uint8_t count_digits_uint(uint16_t val) {
  uint8_t digits = 1;
  
  while ((val /= 10) > 0) {
    digits++;
  }

  return digits;
}
