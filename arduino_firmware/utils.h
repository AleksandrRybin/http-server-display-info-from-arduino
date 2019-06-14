#pragma once

#include <Arduino.h>

// посчитать кол-во цифр в числе
// необходимо для подсчёта Content-Length в запросе
uint8_t count_digits_uint(uint64_t val);