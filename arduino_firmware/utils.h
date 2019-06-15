#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

void http_send_post(EthernetClient& ethernet_client, uint16_t num_touches, uint16_t num_vibrs, uint16_t num_detected, boolean first_connected, uint64_t time);

// посчитать кол-во цифр в числе
// необходимо для подсчёта Content-Length в запросе
uint8_t count_digits_uint(uint64_t val);