#include <DallasTemperature.h>
#include <OneWire.h>
#include "ds18b20Task.h"
#include "mqttTask.h"

using std::string;

OneWire oneWire(GPIO_NUM_12);
DallasTemperature sensors(&oneWire);

void ds18b20Task(void *pvParam) {
  sensors.begin();
  sensors.setWaitForConversion(false);
  while (true) {
    sensors.requestTemperatures();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    float tempEC = sensors.getTempCByIndex(0);
    if (tempEC != DEVICE_DISCONNECTED_C) {
      mqttMessage msg = temperatureToMessage(tempEC);
      // printf("tempEC = %s C\n", msg.metric.c_str());
      xQueueSend(mqttQueue, &msg, portMAX_DELAY);
    } 
    else {
      ESP_LOGE(TAG, "Could not read temperature data");
    }
    vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);
  }
}

mqttMessage temperatureToMessage(float temperature) {
  char data[8];
  sprintf(data, "%.2f", temperature);
  string metric = std::string(data);
  mqttMessage msg = {"tempEC", metric};
  return msg;
}
