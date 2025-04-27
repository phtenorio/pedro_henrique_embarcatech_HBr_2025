#include "hal_temp_sensor.h"
#include "adc_temp_sensor.h"
#include "adc_to_celsius.h"

void hal_temp_sensor_init(void) {
    adc_temp_sensor_init();
}

float hal_temp_sensor_read_celsius(void) {
    uint16_t raw = adc_temp_sensor_read_raw();
    return adc_to_celsius(raw);
}
