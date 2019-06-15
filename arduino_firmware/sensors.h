#pragma once

#include <TM74HC595Display.h>

boolean check_vibr(uint8_t vibr_pin, uint64_t time);

boolean check_touch(uint8_t touch_pin, uint64_t time);

void check_light(boolean reinstall_timer, uint8_t green_light_pin, uint8_t blue_light_pin, uint8_t red_light_pin, uint64_t time);

void light_green(uint8_t green_light_pin, uint8_t blue_light_pin, uint8_t red_light_pin);

void light_blue(uint8_t green_light_pin, uint8_t blue_light_pin, uint8_t red_light_pin);

void light_red(uint8_t green_light_pin, uint8_t blue_light_pin, uint8_t red_light_pin);

void check_display(TM74HC595Display& display, uint64_t time);