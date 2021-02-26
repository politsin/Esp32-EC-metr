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
  mqttQueue = xQueueCreate(10, sizeof(metricMessage));
  xTaskCreate(mqttTask, "mqtt", 1024 * 64, NULL, 1, &mqttHahdle);
  xTaskCreate(blinkTask, "blink", 1024 * 16, NULL, 1, &blink);
  xTaskCreate(ecMetrTask, "ecMetr", 1024 * 32, NULL, 1, &ecMetr);
  xTaskCreate(ds18b20Task, "ds18b20", 1024 * 16, NULL, 1, &ds18b20);
}

char data[16];
int16_t count = 0;
uint16_t loopDelay = 60; // sec.
void loop() {
  if ((count++ % 10) == true) {
    sprintf(data, "%d", count);
    metricMessage msg = {"count", std::string(data)};
    xQueueSend(mqttQueue, &msg, portMAX_DELAY);
  }
  vTaskDelay(loopDelay * 1000 / portTICK_PERIOD_MS);
  printf("Loop %ds | %d | HELLO=%s\n", loopDelay, count, HELLO);
}

