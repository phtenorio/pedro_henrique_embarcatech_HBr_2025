#include <stdio.h>
#include "pico/stdlib.h"
#include "hal_temp_sensor.h"

int main() {
    stdio_init_all();
    hal_temp_sensor_init();

    while (1) {
        float temp = hal_temp_sensor_read_celsius();
        printf("Temperatura: %.2f C\n", temp);
        sleep_ms(1000);
    }
}
