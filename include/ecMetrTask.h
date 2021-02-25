#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

extern TaskHandle_t ecMetr;
void ecMetrTask(void *pvParam);

typedef struct {
  uint16_t positive;
  uint16_t negative;
  float result;
} ec_data_t;

ec_data_t calc(gpio_num_t digital1, gpio_num_t digital2, gpio_num_t adc,
               uint16_t count);

void ecSetPositive();
void ecSetNegative();
void ecSetOff();
uint32_t ecAdc();
ec_data_t calcLoop(uint16_t count);

void configureD32forTermistor();
void configureD33forEC();
