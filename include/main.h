#pragma once
#include <Arduino.h>

// Queue.
extern QueueHandle_t mqttQueue;

using std::string;
typedef struct {
  string name;
  string metric;
} mqttMessage;
