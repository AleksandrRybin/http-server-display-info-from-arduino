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
#define DISP_AWAKE_TIME_MS 3         // таймаут подачи питания на дисплей
#define VIBRATION_CHECK_TIME_MS 40   // таймаут проверки датчика вибрации
#define TOUCH_CHECK_TIME_MS 125      // таймаут проверки датчика касания
#define LIGHT_TIME_MS 165            // время на которое загорается светодиод после обнаружения события

#define SOUND_TIME_MS LIGHT_TIME_MS   // время на которое включается звуковой сигнал после обнаружения события
#define SOUND_TOUCH_FREQ_HZ 1200      // частота звукового сигнала после обнаружения касания
#define SOUND_VIBR_FREQ_HZ 2500       // частота звукового сигнала после обнаружения вибрации

#include <SPI.h>
#include <Ethernet.h>

#include <TM74HC595Display.h>

#include "play_melody.h"
#include "utils.h"
#include "sensors.h"

TM74HC595Display disp(SCLK_PIN, RCLK_PIN, DIO_PIN);  // дисплей
EthernetClient client;                               // клиент Ethernet

void setup() {
  // если кабель не отключен
  if (Ethernet.linkStatus() != LinkOFF) {
    byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // из урока по Ethernet Arduino
    Ethernet.begin(mac); // настройка через DHCP
    delay(2000);

    // отправить http post запрос с начальными данными
    http_send_post(client, 0, 0, 0, true, HTTP_POST_SEND_TIME_MS);
  }
  
  // настройка пинов
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(TOUCH_PIN, INPUT);

  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(GREEN_LIGHT_PIN, OUTPUT);
  pinMode(BLUE_LIGHT_PIN, OUTPUT);
  pinMode(SOUND_PIN, OUTPUT);

  disp.clear();                                                 // очистить дисплей
  play_melody(INIT_MELODY, SOUND_PIN);                          // играть начальную мелодию
  light_red(GREEN_LIGHT_PIN, BLUE_LIGHT_PIN, RED_LIGHT_PIN);    // включить красный светодиод
  disp.digit4(0);                                               // вывести 0 на дисплей
}

void loop() {
  static uint16_t num_touches  = 0;  // количество касаний
  static uint16_t num_vibrs    = 0;  // количество вибраций
  static uint16_t num_detected = 0;  // всего событий

  // проверить таймер выключения индикации события на светодиоде
  check_light(false, GREEN_LIGHT_PIN, BLUE_LIGHT_PIN, RED_LIGHT_PIN, LIGHT_TIME_MS);

  // проверить таймер подачи питания на дисплей
  check_display(disp, DISP_AWAKE_TIME_MS);

  // проверить датчик касаний
  boolean touch = check_touch(TOUCH_PIN, TOUCH_CHECK_TIME_MS);
  if (touch) {
    num_touches++;
    
    // включить только синий светодиод
    light_blue(GREEN_LIGHT_PIN, BLUE_LIGHT_PIN, RED_LIGHT_PIN);

    // подать соотвутствующий звуковой сигнал
    tone(SOUND_PIN, SOUND_TOUCH_FREQ_HZ, SOUND_TIME_MS);
  }

  // проверить датчик вибраций
  boolean vibr = check_vibr(VIBRATION_PIN, VIBRATION_CHECK_TIME_MS);
  if (vibr) {
    num_vibrs++;
    
    // включить только зелёный светодиод
    light_green(GREEN_LIGHT_PIN, BLUE_LIGHT_PIN, RED_LIGHT_PIN);
    
    // подать соотвутствующий звуковой сигнал
    tone(SOUND_PIN, SOUND_VIBR_FREQ_HZ, SOUND_TIME_MS);
  }

  // если произошло какое-то событие
  if (touch || vibr) {
    num_detected = num_vibrs + num_touches;

    // отобразить текущее кол-во событий на дисплей
    disp.digit4(num_detected);
    
     // отправить данные на сервер
    http_send_post(client, num_touches, num_vibrs, num_detected, false, HTTP_POST_SEND_TIME_MS);
    
    // сбросить счётчик индикации на светодиоде после события
    check_light(true, GREEN_LIGHT_PIN, BLUE_LIGHT_PIN, RED_LIGHT_PIN, LIGHT_TIME_MS);
    
    // если число кратно 10 или 100 проиграть особую мелодию
    if (num_detected % 10 == 0 && num_detected != 0 && num_detected % 100 != 0) {
      play_melody(GAIN_10_MELODY, SOUND_PIN);
    } else if (num_detected % 100 == 0 && num_detected != 0) {
      play_melody(GAIN_100_MELODY, SOUND_PIN);
    }
  }
}
