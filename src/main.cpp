#include <Arduino.h>
#include <blinkTask.h>
#include <ds18b20Task.h>
#include <mqttTask.h>
#include <ecMetrTask.h>
#include <main.h>

using std::string;

TaskHandle_t mqttHahdle;
TaskHandle_t ecMetr;
TaskHandle_t ds18b20;
QueueHandle_t mqttQueue;
void setup() {
  ESP_LOGI(TAG, "setup");
  mqttQueue = xQueueCreate(10, sizeof(mqttMessage));
  xTaskCreate(mqttTask, "mqtt", 1024 * 64, NULL, 1, &mqttHahdle);
  xTaskCreate(blinkTask, "blink", 1024 * 16, NULL, 1, &blink);
  xTaskCreate(ecMetrTask, "ecMetr", 1024 * 32, NULL, 1, &ecMetr);
  xTaskCreate(ds18b20Task, "ds18b20", 1024 * 16, NULL, 1, &ds18b20);
}

int16_t count = 0;
void loop() {
  count++;
  uint16_t delay = 5; // sec.
  vTaskDelay(delay * 1000 / portTICK_PERIOD_MS);
  printf("Loop %ds | %d | HELLO=%s\n", delay, count, HELLO);
}

