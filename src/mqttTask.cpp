#include <DallasTemperature.h>
#include <OneWire.h>
#include <freertos/queue.h>
#include <main.h>
#include <mqttTask.h>

using std::string;

EspMQTT mqtt;
mqttMessage msg;
void mqttTask(void *pvParam) {
  uint16_t debugLevel = 0;
  if (debugLevel) {
    mqtt.debugLevel = debugLevel;
    mqtt.setAvailabilityInterval(5);
  }
  
  // MqTT:
  mqtt.setWiFi(WIFI_NAME, WIFI_PASS, WIFI_HOST);
  mqtt.setMqtt(MQTT_SERVER, MQTT_USER, MQTT_PASS);
  mqtt.setCommonTopics(MQTT_ROOT, MQTT_DEVICE);
  mqtt.setCallback(mqtt_callback);
  mqtt.start(true);

  while (true) {
    if (xQueueReceive(mqttQueue, &msg, portMAX_DELAY) == pdTRUE) {
      mqtt.publishMetric(msg.name, msg.metric);
      printf("mqtt [%s] push = %s\n", msg.name.c_str(), msg.metric.c_str());
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// Command callback.
void mqtt_callback(std::string param, std::string value) {
  uint16_t val = atoi(value.c_str());
  printf("%s=%s [%d]\n", param.c_str(), value.c_str(), val);
}
