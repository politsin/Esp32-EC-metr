#pragma once
#include <Arduino.h>

// Queue.
extern QueueHandle_t mqttQueue;

using std::string;
typedef struct {
  string name;
  string metric;
} mqttMessage;

// Log.
#include "esp_err.h"
#include "esp_log.h"
#define TAG "LOG"
