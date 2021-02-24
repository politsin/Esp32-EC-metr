#include <DallasTemperature.h>
#include <OneWire.h>
#include <freertos/queue.h>
#include <main.h>

using std::string;

OneWire oneWire(GPIO_NUM_23);
DallasTemperature sensors(&oneWire);

void ds18b20Task(void *pvParam) {
  sensors.begin();
  sensors.setWaitForConversion(false);
  while (true) {
    sensors.requestTemperatures();
    vTaskDelay(750 / portTICK_PERIOD_MS);
    float tempEC = sensors.getTempCByIndex(0);
    if (tempEC != DEVICE_DISCONNECTED_C) {
      char data[8];
      sprintf(data, "%.2f", tempEC);
      string metric = std::string(data);
      mqttMessage msg = {"tin", metric};
      xQueueSend(mqttQueue, &msg, portMAX_DELAY);
      printf("tempEC = %.2f C\n", tempEC);
    } else {
      printf("Error: Could not read temperature data\n");
    }
    vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);
  }
}
