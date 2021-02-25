#include <driver/adc.h>
#include <driver/gpio.h>
#include <esp_adc_cal.h>
#include "ecMetrTask.h"
#include "mqttTask.h"

using std::string;

#define ON 1
#define OFF 0

static const uint32_t reference_voltage = 3300;
static const adc_channel_t channelEc = ADC_CHANNEL_5;
static const adc_channel_t channelTermistor = ADC_CHANNEL_4;
esp_adc_cal_characteristics_t charsEc;
esp_adc_cal_characteristics_t charsTermistor;

static const uint16_t count = 500;
static const gpio_num_t adc = GPIO_NUM_33;
static const gpio_num_t termistor = GPIO_NUM_32;
static const gpio_num_t digital1 = GPIO_NUM_18;
static const gpio_num_t digital2 = GPIO_NUM_19;

void ecMetrTask(void *pvParam) {
  // configureD32forTermistor();
  configureD33forEC();
  gpio_pad_select_gpio(digital1);
  gpio_pad_select_gpio(digital2);
  gpio_set_direction(digital1, GPIO_MODE_OUTPUT);
  gpio_set_direction(digital2, GPIO_MODE_OUTPUT);
  ecSetOff();

  ec_data_t ec;
  portMUX_TYPE ecMutex = portMUX_INITIALIZER_UNLOCKED;
  while (true) {
    portENTER_CRITICAL(&ecMutex);
    ec = calcLoop(20);
    portEXIT_CRITICAL(&ecMutex);
    printf("ADC: %.2f mV [%u|%u]\n", ec.result, ec.positive, ec.negative);
    char data[60];
    sprintf(data, "%.2f", ec.result);
    string metric = std::string(data);
    mqttMessage msg = {"ec", metric};
    xQueueSend(mqttQueue, &msg, portMAX_DELAY);
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
    negative += reference_voltage - ecAdc();
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
  esp_adc_cal_get_voltage(channelEc, &charsEc, &volt);
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
 * Coficure D32 Analog Input (Termistor).
 */
void configureD32forTermistor() {
  static const adc_unit_t unit = ADC_UNIT_1;
  static const adc_atten_t atten = ADC_ATTEN_DB_11;
  static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
  adc1_config_width(width);
  adc1_config_channel_atten((adc1_channel_t)channelTermistor, atten);
  esp_adc_cal_characterize(unit, atten, width, reference_voltage, &charsTermistor);
}

/**
 * Coficure D3e Analog Input (EC).
 */
void configureD33forEC() {
  static const adc_unit_t unit = ADC_UNIT_1;
  static const adc_atten_t atten = ADC_ATTEN_DB_11;
  static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
  adc1_config_width(width);
  adc1_config_channel_atten((adc1_channel_t)channelEc, atten);
  esp_adc_cal_characterize(unit, atten, width, reference_voltage, &charsEc);
}

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
