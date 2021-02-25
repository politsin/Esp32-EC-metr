#pragma once
#include "mqttTask.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

extern TaskHandle_t ds18b20;
void ds18b20Task(void *pvParam);

mqttMessage temperatureToMessage(float temperature);
