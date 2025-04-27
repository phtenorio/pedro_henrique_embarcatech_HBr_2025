#ifndef ADC_TEMP_SENSOR_H
#define ADC_TEMP_SENSOR_H

#include <stdint.h>

void adc_temp_sensor_init(void);
uint16_t adc_temp_sensor_read_raw(void);

#endif
