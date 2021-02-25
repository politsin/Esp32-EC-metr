#pragma once
#include "EspMQTT.h"
#include <Arduino.h>
#include <main.h>

extern TaskHandle_t mqttHahdle;
void mqttTask(void *pvParam);
void mqtt_callback(std::string param, std::string value);

// Queue.
extern QueueHandle_t mqttQueue;
typedef struct {
  string name;
  string metric;
} mqttMessage;
