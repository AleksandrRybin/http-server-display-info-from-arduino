#include <Arduino.h>

#include "utils.h"

uint8_t count_digits_uint(uint64_t val) {
  uint8_t digits = 1;
  
  while ((val /= 10) > 0) {
    digits++;
  }

  return digits;
}

void http_send_post(EthernetClient& ethernet_client, uint16_t num_touches, uint16_t num_vibrs, uint16_t num_detected, boolean first_connected, uint64_t time) {
  static uint64_t http_send_timer = 0;

  if (millis() - http_send_timer >= time) {
    if (Ethernet.linkStatus() != LinkOFF) {
      if (ethernet_client.connect("afternoon-ravine-88100.herokuapp.com", 80)) {
        uint16_t content_length = 0;
        content_length += strlen("{}");
        content_length += strlen("\"touches\":,");
        content_length += strlen("\"vibrs\":,");
        content_length += strlen("\"total\":,");
        content_length += strlen("\"first_connected\":");
      
        ethernet_client.println("POST /update HTTP/1.1");
        ethernet_client.println("Host: afternoon-ravine-88100.herokuapp.com");
        ethernet_client.println("Content-Type: application/json");
        
        ethernet_client.print("Content-Length: ");
        
        content_length += count_digits_uint(num_touches);
        content_length += count_digits_uint(num_vibrs);
        content_length += count_digits_uint(num_detected);
        content_length += first_connected ? strlen("true") : strlen("false");

        ethernet_client.println(content_length + 2);
        ethernet_client.println();
      
        ethernet_client.print("{");
        
        ethernet_client.print("\"touches\":");
        ethernet_client.print(num_touches);
        ethernet_client.print(",");

        ethernet_client.print("\"vibrs\":");
        ethernet_client.print(num_vibrs);
        ethernet_client.print(",");

        ethernet_client.print("\"total\":");
        ethernet_client.print(num_detected);
        ethernet_client.print(",");
        
        ethernet_client.print("\"first_connected\":");
        if (first_connected) {
          ethernet_client.print("true");
        } else {
          ethernet_client.print("false");
        }
        
        ethernet_client.println("}");

        ethernet_client.println("Connection: close");
        ethernet_client.println();
      }
    }
    
    http_send_timer = millis();
  }  
}
