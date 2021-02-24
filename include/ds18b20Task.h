#pragma once
#include <main.h>
#include <Arduino.h>

extern TaskHandle_t ds18b20;
void ds18b20Task(void *pvParam);

mqttMessage temperatureToMessage(float temperature);
