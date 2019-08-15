#include <Arduino.h>

#include "sensors.h"

boolean check_vibr(uint8_t vibr_pin, uint64_t time) {
  static uint64_t vibr_check_timer = 0;
  static boolean is_vibr = false;

  if (millis() - vibr_check_timer >= time) {
    if (digitalRead(vibr_pin) == HIGH) {
      if (!is_vibr) {
        is_vibr = true;
        vibr_check_timer = millis();
        
        return true;
      }
    } else if (is_vibr){
      is_vibr = false;
    }
    
    vibr_check_timer = millis();
  }

  return false;
}

boolean check_touch(uint8_t touch_pin, uint64_t time) {
  static uint64_t touch_check_timer = 0;
  static boolean is_touch = false;

  if (millis() - touch_check_timer >= time) {
    if (digitalRead(touch_pin) == HIGH) {
      if (!is_touch) {
        is_touch = true;
        touch_check_timer = millis();

        return true;
      }
    } else if (is_touch){
      is_touch = false;
    }
    
    touch_check_timer = millis();
  }

  return false;
}

void check_light(boolean reinstall_timer, uint8_t green_light_pin, uint8_t blue_light_pin, uint8_t red_light_pin, uint64_t time) {
  static uint64_t light_timer = 0;

  // если требуется сбросить таймер
  if (reinstall_timer) {
    light_timer = millis();
  }
  
  if (millis() - light_timer >= time) {
    light_red(green_light_pin, blue_light_pin, red_light_pin);

    light_timer = millis();
  }
}

void light_green(uint8_t green_light_pin, uint8_t blue_light_pin, uint8_t red_light_pin) {
  digitalWrite(red_light_pin, LOW);
  digitalWrite(blue_light_pin, LOW);
  digitalWrite(green_light_pin, HIGH);
}

void light_blue(uint8_t green_light_pin, uint8_t blue_light_pin, uint8_t red_light_pin) {
  digitalWrite(red_light_pin, LOW);
  digitalWrite(green_light_pin, LOW);
  digitalWrite(blue_light_pin, HIGH);
}

void light_red(uint8_t green_light_pin, uint8_t blue_light_pin, uint8_t red_light_pin) {
  digitalWrite(green_light_pin, LOW);
  digitalWrite(blue_light_pin, LOW);
  digitalWrite(red_light_pin, HIGH);
}

void check_display(TM74HC595Display& display, uint64_t time) {
  static uint64_t disp_isr_timer = 0;
  
  if (millis() - disp_isr_timer >= time) {
    display.timerIsr();       
    disp_isr_timer = millis();
  }
}
