#include <Arduino.h>

#include "utils.h"

uint8_t count_digits_uint(uint64_t val) {
  uint8_t digits = 1;
  
  while ((val /= 10) > 0) {
    digits++;
  }

  return digits;
}
