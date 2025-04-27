#include "adc_to_celsius.h"

#define SENSOR_VOLTAGE_27C 0.706f
#define TEMP_SLOPE 0.001721f
#define ADC_MAX 4095.0f   // ADC de 12 bits (0-4095)
#define VREF 3.3f

float adc_to_celsius(uint16_t adc_val) {
    float voltage;

    voltage = 27.0f - (((adc_val * VREF / ADC_MAX) - SENSOR_VOLTAGE_27C) / TEMP_SLOPE);

    return voltage;
}
