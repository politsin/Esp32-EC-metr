#include <DallasTemperature.h>
#include <OneWire.h>
#include "ds18b20Task.h"
#include "mqttTask.h"

using std::string;

OneWire oneWire(GPIO_NUM_23);
DallasTemperature sensors(&oneWire);
const uint8_t bitResolution = 12;

void ds18b20Task(void *pvParam) {
  DeviceAddress deviceAddress;
  sensors.begin();
  sensors.setWaitForConversion(false);
  uint8_t delay = waitForAddresAndSetDelay(sensors, deviceAddress);
  while (true) {
    sensors.requestTemperatures();
    vTaskDelay(delay / portTICK_PERIOD_MS);
    float temperature = sensors.getTempC((uint8_t *)deviceAddress);
    if (temperature != DEVICE_DISCONNECTED_C) {
      string metric = temperatureToSting(temperature);
      mqttMessage msg = {"temperature", metric};
      xQueueSend(mqttQueue, &msg, portMAX_DELAY);
    } 
    else {
      ESP_LOGE(TAG, "Could not read temperature data");
    }
    vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);
  }
}

// Delay.
uint8_t waitForAddresAndSetDelay(DallasTemperature sensors,
                                 DeviceAddress deviceAddress) {
  bool connected = false;
  while (!connected) {
    connected = sensors.getAddress(deviceAddress, 0);
    if (!connected) {
      ESP_LOGE(TAG, "Could not find ds18b20");
    } 
    else {
      sensors.setResolution((uint8_t *)deviceAddress, 12);
    }
    vTaskDelay(2 * 1000 / portTICK_PERIOD_MS);
  }
  uint8_t delay = sensors.millisToWaitForConversion(bitResolution);
  return delay;
}

// Float To String.
string temperatureToSting(float temperature) {
  char data[8];
  sprintf(data, "%.2f", temperature);
  string metric = std::string(data);
  return metric;
}
