#include <Arduino.h>
// #include <blinkTask.h> WTF o.O
// Blink #19.
#include "driver/gpio.h"
#define LED_PIN GPIO_NUM_19

void blinkTask(void *pvParam) {
  gpio_pad_select_gpio(LED_PIN);
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
  int isOn = 0;
  while (true) {
    gpio_set_level(LED_PIN, isOn);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_set_level(LED_PIN, !isOn);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}
