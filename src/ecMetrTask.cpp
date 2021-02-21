#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_adc_cal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/adc.h>
#include <ecMetrTask.h>

#define ON 1
#define OFF 0

#define ANALOG_PIN GPIO_NUM_35 // ADC1_CHANNEL_6_GPIO_NUM.
#define VV2 3300               // ADC reference voltage
static const adc_unit_t unit = ADC_UNIT_1;
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_channel_t channel = ADC_CHANNEL_7;
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
esp_adc_cal_characteristics_t chars;

static const uint16_t count = 500;
static const gpio_num_t adc = GPIO_NUM_35;
static const gpio_num_t digital1 = GPIO_NUM_16;
static const gpio_num_t digital2 = GPIO_NUM_17;

void ecMetrTask(void *pvParam) {

  adc1_config_width(width);
  adc1_config_channel_atten((adc1_channel_t)channel, atten);
  esp_adc_cal_characterize(unit, atten, width, VV2, &chars);
  gpio_pad_select_gpio(digital1);
  gpio_pad_select_gpio(digital2);
  gpio_set_direction(digital1, GPIO_MODE_OUTPUT);
  gpio_set_direction(digital2, GPIO_MODE_OUTPUT);
  ecSetOff();

  ec_data_t ec;
  portMUX_TYPE ecMutex = portMUX_INITIALIZER_UNLOCKED;
  while (true) {
    portENTER_CRITICAL(&ecMutex);
    ec = calcLoop(200);
    portEXIT_CRITICAL(&ecMutex);
    printf("ADC: %.2f mV [%u|%u]\n", ec.result, ec.positive, ec.negative);
    vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);
  }
}

ec_data_t calcLoop(uint16_t count) {
  ec_data_t result = {0};
  uint32_t positive = 0;
  uint32_t negative = 0;
  ecSetOff();
  for (uint16_t i = 0; i < count; i++) {
    // ets_delay_us(1 * 1000);
    ecSetPositive();
    positive += ecAdc();
    ecSetNegative();
    negative += ecAdc();
    ecSetOff();
  }
  ecSetOff();
  result.positive = (uint32_t)positive / count;
  result.negative = (uint32_t)negative / count;
  result.result = 0.5 * (positive + negative) / count;
  return result;
}

uint32_t ecAdc() {
  uint32_t volt;
  esp_adc_cal_get_voltage(channel, &chars, &volt);
  return volt;
}

void ecSetOff() {
  gpio_set_level(digital1, OFF);
  gpio_set_level(digital2, OFF);
}
void ecSetPositive() {
  gpio_set_level(digital1, OFF);
  gpio_set_level(digital2, ON);
}
void ecSetNegative() {
  gpio_set_level(digital1, ON);
  gpio_set_level(digital2, OFF);
}

/**
 *  Calibrate.
 */
float fCalibration(float x1, float ec1, float x2, float ec2, float x) {
  float b = (-log(ec1 / ec2)) / log(x2 / x1);
  float a = ((pow(x1, -b)) * ec1);
  float ec0 = a * pow(x, b);
  return ec0;
}

/**
 *  Термокомпенсация ЕС.
 */
float fECTemp(float k, float ec0, float t) { return ec0 / (1 + k * (t - 25)); }


/**
 * typedef enum {
 ADC1_CHANNEL_0 = 0, !< ADC1 channel 0 is GPIO36
 ADC1_CHANNEL_1,     !< ADC1 channel 1 is GPIO37
 ADC1_CHANNEL_2,     !< ADC1 channel 2 is GPIO38
 ADC1_CHANNEL_3,     !< ADC1 channel 3 is GPIO39
 ADC1_CHANNEL_4,     !< ADC1 channel 4 is GPIO32
 ADC1_CHANNEL_5,     !< ADC1 channel 5 is GPIO33
 ADC1_CHANNEL_6,     !< ADC1 channel 6 sis GPIO34
 ADC1_CHANNEL_7,     !< ADC1 channel 7 is GPIO35
 ADC1_CHANNEL_MAX,
 } adc1_channel_t;

 typedef enum {
 ADC2_CHANNEL_0 = 0, !< ADC2 channel 0 is GPIO4
 ADC2_CHANNEL_1,     !< ADC2 channel 1 is GPIO0
 ADC2_CHANNEL_2,     !< ADC2 channel 2 is GPIO2
 ADC2_CHANNEL_3,     !< ADC2 channel 3 is GPIO15
 ADC2_CHANNEL_4,     !< ADC2 channel 4 is GPIO13
 ADC2_CHANNEL_5,     !< ADC2 channel 5 is GPIO12
 ADC2_CHANNEL_6,     !< ADC2 channel 6 is GPIO14
 ADC2_CHANNEL_7,     !< ADC2 channel 7 is GPIO27
 ADC2_CHANNEL_8,     !< ADC2 channel 8 is GPIO25
 ADC2_CHANNEL_9,     !< ADC2 channel 9 is GPIO26
 ADC2_CHANNEL_MAX,
 } adc2_channel_t;
 */
