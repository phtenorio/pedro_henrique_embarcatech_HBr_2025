#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#define I2C_PORT i2c1
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15
#define OLED_ADDRESS 0x3C
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define MOVING_AVG_SIZE 20

ssd1306_t oled;

typedef enum {MODE_C, MODE_F, MODE_K} temp_mode_t;
temp_mode_t mode = MODE_C;

// Buffer para média móvel
float temp_buffer[MOVING_AVG_SIZE];
uint8_t temp_index = 0;
uint8_t temp_count = 0;

// Função de média móvel
float moving_average(float new_sample) {
    temp_buffer[temp_index] = new_sample;
    temp_index = (temp_index + 1) % MOVING_AVG_SIZE;
    if (temp_count < MOVING_AVG_SIZE) temp_count++;

    float sum = 0.0f;
    for (uint8_t i = 0; i < temp_count; i++) {
        sum += temp_buffer[i];
    }
    return sum / temp_count;
}

float read_internal_temperature() {
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
    uint16_t raw = adc_read();
    float voltage = raw * 3.3f / (1 << 12);
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temperature;
}

float convert_temp(float celsius, temp_mode_t mode) {
    if (mode == MODE_F)
        return celsius * 9.0f / 5.0f + 32.0f;
    else if (mode == MODE_K)
        return celsius + 273.15f;
    else
        return celsius;
}

const char* get_unit(temp_mode_t mode) {
    if (mode == MODE_F) return "F";
    if (mode == MODE_K) return "K";
    return "C";
}

// Termômetro visual mais bonito
void draw_thermometer(float temp_c) {
    // Escala visual
    const float temp_min = 15.0f;
    const float temp_max = 60.0f;

    // Tubo do termômetro
    const int x = 108, y = 8, w = 8, h = 44;

    // Bulbo
    const int bulbo_x = x + w/2, bulbo_y = y + h + 7, bulbo_r = 7;

    // Desenha tubo (bordas)
    ssd1306_draw_rect(&oled, x, y, w, h);

    // Desenha escala (5 marcas)
    for(int i=0; i<=5; i++) {
        int mark_y = y + h - (i * h)/5;
        ssd1306_draw_rect(&oled, x+w+1, mark_y-1, 3, 2);
    }

    // Calcula preenchimento do "mercúrio"
    float pct = (temp_c - temp_min) / (temp_max - temp_min);
    if (pct < 0) pct = 0;
    if (pct > 1) pct = 1;
    int fill_h = (int)(pct * (h-4));
    // Preenche tubo com barra arredondada
    for(int i=0; i<w-2; i++) {
        for(int j=0; j<fill_h; j++) {
            int yy = y + h - 2 - j;
            ssd1306_draw_pixel(&oled, x+1+i, yy);
        }
    }
    // Bulbo preenchido
    ssd1306_fill_circle(&oled, bulbo_x, bulbo_y, bulbo_r);
    // Contorno do bulbo
    ssd1306_draw_circle(&oled, bulbo_x, bulbo_y, bulbo_r);
}

void update_display(float temp_c, temp_mode_t mode) {
    char buf[24];
    ssd1306_clear(&oled);

    // Área útil para centralizar texto: x=0 até x=95 (largura=96)
    // Centraliza "Temp.:" (fonte 1, 6px por char, "Temp.:"=36px, centraliza em x=30)
    ssd1306_draw_string(&oled, 30, 8, 1, "Temp.:");

    // Valor centralizado, fonte grande (font size 2, 12px por char)
    float temp_disp = convert_temp(temp_c, mode);
    snprintf(buf, sizeof(buf), "%.1f %s", temp_disp, get_unit(mode));
    int len = strlen(buf);
    int x_center = 48 - (len * 12) / 2; // Centraliza na área útil (0-95)
    if (x_center < 0) x_center = 0;
    ssd1306_draw_string(&oled, x_center, 26, 2, buf);

    // Termômetro gráfico à direita
    draw_thermometer(temp_c);

    ssd1306_show(&oled);
}

bool button_pressed(uint pin) {
    static uint32_t last_a = 0, last_b = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (pin == BUTTON_A_PIN && gpio_get(pin) == 0 && now - last_a > 300) {
        last_a = now;
        return true;
    }
    if (pin == BUTTON_B_PIN && gpio_get(pin) == 0 && now - last_b > 300) {
        last_b = now;
        return true;
    }
    return false;
}

int main() {
    stdio_init_all();

    // Inicializa I2C e OLED
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    ssd1306_init(&oled, 128, 64, OLED_ADDRESS, I2C_PORT);

    adc_init();

    // Inicializa botões
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Inicializa buffer da média móvel
    for (int i = 0; i < MOVING_AVG_SIZE; i++) temp_buffer[i] = read_internal_temperature();

    while (true) {
        float temp_raw = read_internal_temperature();
        float temp_c = moving_average(temp_raw);

        // Troca modo se botão A ou B for pressionado
        if (button_pressed(BUTTON_A_PIN) || button_pressed(BUTTON_B_PIN)) {
            mode = (mode + 1) % 3;
        }

        update_display(temp_c, mode);
        sleep_ms(200);
    }
}
