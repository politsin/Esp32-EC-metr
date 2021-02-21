#pragma once
#ifndef __BLINK_TASK_H
#define __BLINK_TASK_H

#include <Arduino.h>
#include "driver/gpio.h"

TaskHandle_t blink;
void blinkTask(void *pvParam);

#endif
