#pragma once
#include <DallasTemperature.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

using std::string;

extern TaskHandle_t ds18b20;
void ds18b20Task(void *pvParam);

string temperatureToSting(float temperature);
uint8_t waitForAddresAndSetDelay(DallasTemperature sensors,
                                 DeviceAddress deviceAddress);
