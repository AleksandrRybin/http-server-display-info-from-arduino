#define SCLK_PIN 6 // 
#define RCLK_PIN 5 // Для библиотеки вывода на дисплей
#define DIO_PIN 4 //

#define VIBRATION_PIN 2 // пин датчика вибрации
#define TOUCH_PIN 3    // пин датчика касания
#define SOUND_PIN A2   // пин динамика

#define RED_LIGHT_PIN A5   // пин красного светодиода
#define GREEN_LIGHT_PIN A3 // пин зелёного светодиода
#define BLUE_LIGHT_PIN A4  // пин синего светодиода

#define HTTP_POST_SEND_TIME_MS 1000  // таймаут отправки http запросов
#define DISP_AWAKE_TIME_MS 3        // таймаут подачи питания на дисплей
#define VIBRATION_CHECK_TIME_MS 40  // таймаут проверки датчика вибрации
#define TOUCH_CHECK_TIME_MS 125     // таймаут проверки датчика касания
#define LIGHT_TIME_MS 165           // время на которое загорается светодиод после обнаружения события

#define SOUND_TIME_MS LIGHT_TIME_MS   // время на которое включается звуковой сигнал после обнаружения события
#define SOUND_TOUCH_FREQ_HZ 1200     // частота звукового сигнала после обнаружения касания
#define SOUND_VIBR_FREQ_HZ 2500     // частота звукового сигнала после обнаружения вибрации

#include <SPI.h>
#include <Ethernet.h>

#include <TM74HC595Display.h>

#include "play_melody.h"
#include "utils.h"

TM74HC595Display disp(SCLK_PIN, RCLK_PIN, DIO_PIN);  // дисплей
EthernetClient client;                               // клиент Ethernet

void setup() {
  // если кабель не отключен
  if (Ethernet.linkStatus() != LinkOFF) {
    byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // из урока по Ethernet Arduino
    Ethernet.begin(mac); // настройка через DHCP
    delay(2000);

    // отправить http post запрос с начальными данными
    http_send_post(0, 0, 0, true);
  }
  
  // настройка пинов
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(TOUCH_PIN, INPUT);

  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(GREEN_LIGHT_PIN, OUTPUT);
  pinMode(BLUE_LIGHT_PIN, OUTPUT);
  pinMode(SOUND_PIN, OUTPUT);

  disp.clear();                           // очистить дисплей
  play_melody(INIT_MELODY, SOUND_PIN);   // играть начальную мелодию
  digitalWrite(RED_LIGHT_PIN, HIGH);    // включить красный светодиод
  disp.digit4(0);                      // вывести 0 на дисплей
}

void loop() {
  static uint16_t num_touches = 0;  // количество касаний
  static uint16_t num_vibrs = 0;    // количество вибраций
  static uint16_t num_detected = 0; // всего событий

  check_light(false); // проверить таймер выключения индикации события на светодиоде
  check_display();    // проверить таймер подачи питания на дисплей              

  // проверить датчик касаний
  boolean touch = check_touch();
  if (touch) {
    num_touches++;
    
    // включить только синий светодиод
    digitalWrite(RED_LIGHT_PIN, LOW);
    digitalWrite(GREEN_LIGHT_PIN, LOW);
    digitalWrite(BLUE_LIGHT_PIN, HIGH);

    // подать соотвутствующий звуковой сигнал
    tone(SOUND_PIN, SOUND_TOUCH_FREQ_HZ, SOUND_TIME_MS);
  }

  // проверить датчик вибраций
  boolean vibr = check_vibr();
  if (vibr) {
    num_vibrs++;
    
    // включить только зелёный светодиод
    digitalWrite(RED_LIGHT_PIN, LOW);
    digitalWrite(BLUE_LIGHT_PIN, LOW);
    digitalWrite(GREEN_LIGHT_PIN, HIGH);
    
    // подать соотвутствующий звуковой сигнал
    tone(SOUND_PIN, SOUND_VIBR_FREQ_HZ, SOUND_TIME_MS);
  }

  // если произошло какое-то событие
  if (touch || vibr) {
    num_detected = num_vibrs + num_touches;
    disp.digit4(num_detected); // отобразить текущее кол-во событий на дисплей
    http_send_post(num_touches, num_vibrs, num_detected, false); // отправить данные на сервер
    check_light(true); // сбросить счётчик индикации на светодиоде после события
    
    // если число кратно 10 или 100 проиграть мелодию
    if (num_detected % 10 == 0 && num_detected != 0 && num_detected % 100 != 0) {
      play_melody(GAIN_10_MELODY, SOUND_PIN);
    }
    else if (num_detected % 100 == 0 && num_detected != 0) {
      play_melody(GAIN_100_MELODY, SOUND_PIN);
    }
  }
}

void http_send_post(uint16_t num_touches, uint16_t num_vibrs, uint16_t num_detected, boolean first_connected) {
  static uint64_t http_send_timer = 0;

  if (millis() - http_send_timer >= HTTP_POST_SEND_TIME_MS) {
    if (Ethernet.linkStatus() != LinkOFF) {
      if (client.connect("afternoon-ravine-88100.herokuapp.com", 80)) {
        uint16_t content_length = 0;
        content_length += strlen("{}");
        content_length += strlen("\"touches\":,");
        content_length += strlen("\"vibrs\":,");
        content_length += strlen("\"total\":,");
        content_length += strlen("\"first_connected\":");
      
        client.println("POST /update HTTP/1.1");
        client.println("Host: afternoon-ravine-88100.herokuapp.com");
        client.println("Content-Type: application/json");
        
        client.print("Content-Length: ");
        
        content_length += count_digits_uint(num_touches);
        content_length += count_digits_uint(num_vibrs);
        content_length += count_digits_uint(num_detected);
        content_length += first_connected ? strlen("true") : strlen("false");

        client.println(content_length + 2);
        client.println();
      
        client.print("{");
        
        client.print("\"touches\":");
        client.print(num_touches);
        client.print(",");

        client.print("\"vibrs\":");
        client.print(num_vibrs);
        client.print(",");

        client.print("\"total\":");
        client.print(num_detected);
        client.print(",");
        
        client.print("\"first_connected\":");
        if (first_connected) {
          client.print("true");
        }
        else {
          client.print("false");
        }
        
        client.println("}");

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

  // если требуется сбросить таймер
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
