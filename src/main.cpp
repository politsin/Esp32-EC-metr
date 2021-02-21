#include <Arduino.h>
#include <blinkTask.h>
#include <ecMetrTask.h>
#include <main.h>

// Log
#include "esp_err.h"
#include "esp_log.h"
#define TAG "LOG"

// EspMqtt32.
#include "Esp32MQTT.h"
Esp32MQTT mqtt;
void mqtt_callback(std::string param, std::string value);
// https://github.com/krzychb/esp32-everest-run/tree/master/components
void mqttSetup() {
  uint16_t debugLevel = 0;
  if (debugLevel) {
    mqtt.debugLevel = debugLevel;
    mqtt.setAvailabilityInterval(5);
  }
  // MqTT:
  mqtt.setWiFi(WIFI_NAME, WIFI_PASS, WIFI_HOST);
  mqtt.setMqtt(MQTT_SERVER, MQTT_USER, MQTT_PASS);
  mqtt.setCommonTopics(MQTT_ROOT, MQTT_DEVICE);
  // mqtt.setCallback(mqtt_callback);
  mqtt.start(true);
}

TaskHandle_t ecMetr;
void setup() {
  mqttSetup();
  ESP_LOGI(TAG, "setup");
  xTaskCreate(blinkTask, "blink", 1024, NULL, 1, &blink);
  xTaskCreate(ecMetrTask, "ecMetr", 1024 * 8, NULL, 1, &ecMetr);
}
mqttMessage msg;
QueueHandle_t mqttQueue;
int16_t count = 0;
void loop() {
  uint16_t delay = 60; // sec.
  vTaskDelay(delay * 1000 / portTICK_PERIOD_MS);
  printf("Loop %ds | HELLO=%s\n", delay, HELLO);
  if (xQueueReceive(mqttQueue, &msg, 100 / portTICK_PERIOD_MS) == pdTRUE) {
    // mqtt.publishMetric(msg.name, msg.metric);
    printf("mqtt [%s] push = %s\n", msg.name.c_str(), msg.metric.c_str());
  }
  // vTaskDelay(300 / portTICK_PERIOD_MS);
}

// Command callback.
void mqtt_callback(std::string param, std::string value) {
  uint16_t val = atoi(value.c_str());
  printf("%s=%s\n", param.c_str(), value.c_str());
}
