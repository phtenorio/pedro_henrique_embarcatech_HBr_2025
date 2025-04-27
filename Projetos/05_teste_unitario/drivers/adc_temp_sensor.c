#include "adc_temp_sensor.h"
#include "hardware/adc.h"

void adc_temp_sensor_init(void) {
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4); // Canal 4 = sensor interno
}

uint16_t adc_temp_sensor_read_raw(void) {
    return adc_read();
}
