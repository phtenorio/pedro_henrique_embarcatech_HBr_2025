#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define JOYSTICK_BUTTON_PIN 22
#define BUTTON_B_PIN 6
#define I2C_PORT i2c1
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15
#define OLED_ADDRESS 0x3C

typedef struct {
    uint16_t min;
    uint16_t center;
    uint16_t max;
} AxisCalibration;

ssd1306_t oled;
AxisCalibration cal_x = {0, 0, 4095};
AxisCalibration cal_y = {0, 0, 4095};

bool debounce_button(uint pin) {
    static uint32_t last_press = 0;
    if (gpio_get(pin) == 0) { // Botão pressionado (pull-up)
        if (to_ms_since_boot(get_absolute_time()) - last_press > 500) {
            last_press = to_ms_since_boot(get_absolute_time());
            return true;
        }
    }
    return false;
}

// Função de mapeamento com clamp (garante 0-100%)
uint16_t map_value_centered(uint16_t v, uint16_t min, uint16_t max, uint16_t center) {
    int16_t range_neg = center - min;
    int16_t range_pos = max - center;
    int percent;
    if (v < center) {
        if (range_neg == 0) percent = 50;
        else percent = 50 * (v - min) / range_neg;
    } else {
        if (range_pos == 0) percent = 50;
        else percent = 50 + 50 * (v - center) / range_pos;
    }
    // Clamp entre 0 e 100
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    return (uint16_t)percent;
}

void show_message(const char* line1, const char* line2) {
    ssd1306_clear(&oled);
    ssd1306_draw_string(&oled, 0, 0, 1, line1);
    ssd1306_draw_string(&oled, 0, 16, 1, line2);
    ssd1306_show(&oled);
}

void calibration_routine() {
    // Passo 1: Esquerda (min X)
    show_message("Empurre para", "ESQUERDA e click B");
    while(!debounce_button(BUTTON_B_PIN)) {
        adc_select_input(JOYSTICK_X_PIN - 26);
        cal_x.min = adc_read();
        sleep_ms(10);
    }

    // Passo 2: Direita (max X)
    show_message("Empurre para", "DIREITA e click B");
    while(!debounce_button(BUTTON_B_PIN)) {
        adc_select_input(JOYSTICK_X_PIN - 26);
        cal_x.max = adc_read();
        sleep_ms(10);
    }

    // Passo 3: Baixo (min Y)
    show_message("Empurre para", "BAIXO e click B");
    while(!debounce_button(BUTTON_B_PIN)) {
        adc_select_input(JOYSTICK_Y_PIN - 26);
        cal_y.min = adc_read();
        sleep_ms(10);
    }

    // Passo 4: Cima (max Y)
    show_message("Empurre para", "CIMA e click B");
    while(!debounce_button(BUTTON_B_PIN)) {
        adc_select_input(JOYSTICK_Y_PIN - 26);
        cal_y.max = adc_read();
        sleep_ms(10);
    }

    // Passo 5: Centralizar (tira a média das leituras enquanto o usuário não pressiona nada)
    show_message("Solte o joy", "para centralizar");
    uint32_t soma_x = 0, soma_y = 0;
    uint16_t count = 0;
    sleep_ms(300); // Pequena pausa para o usuário soltar o joystick
    for (int i = 0; i < 100; i++) {
        adc_select_input(JOYSTICK_X_PIN - 26);
        soma_x += adc_read();
        adc_select_input(JOYSTICK_Y_PIN - 26);
        soma_y += adc_read();
        count++;
        sleep_ms(5);
    }
    cal_x.center = soma_x / count;
    cal_y.center = soma_y / count;

    show_message("Calibracao", "completa!");
    sleep_ms(1000);
}

void update_display(uint16_t x, uint16_t y, bool btn) {
    char buf[24];
    ssd1306_clear(&oled);

    // Eixo X
    uint16_t x_percent = map_value_centered(x, cal_x.min, cal_x.max, cal_x.center);
    snprintf(buf, sizeof(buf), "X:%3d%%", x_percent);
    ssd1306_draw_string(&oled, 0, 0, 1, buf);

    // Eixo Y
    uint16_t y_percent = map_value_centered(y, cal_y.min, cal_y.max, cal_y.center);
    snprintf(buf, sizeof(buf), "Y:%3d%%", y_percent);
    ssd1306_draw_string(&oled, 64, 0, 1, buf);

    // Use os percentuais já limitados para a posição do ponto
    uint8_t pos_x = (x_percent * 120)/100;
    uint8_t pos_y = 63 - (y_percent * 60)/100;
    ssd1306_fill_circle(&oled, pos_x, pos_y, 3);

    // Botão
    ssd1306_draw_string(&oled, 0, 56, 1, btn ? "[BTN]" : "[----]");

    // Mensagem de calibração no canto inferior direito
    ssd1306_draw_string(&oled, 80, 56, 1, "B=Calib");

    ssd1306_show(&oled);
}


int main() {
    stdio_init_all();

    // Inicializa I2C e OLED
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    ssd1306_init(&oled, 128, 64, OLED_ADDRESS, I2C_PORT);

    // Inicializa entradas
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    gpio_init(JOYSTICK_BUTTON_PIN);
    gpio_set_dir(JOYSTICK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Verifica se o botão B está pressionado no início para calibrar
    if(gpio_get(BUTTON_B_PIN) == 0) {
        calibration_routine();
    }

    // Loop principal
    while(true) {
        // Verifica se quer recalibrar
        if(debounce_button(BUTTON_B_PIN)) {
            calibration_routine();
        }

        // Leitura do joystick
        adc_select_input(JOYSTICK_X_PIN - 26);
        uint16_t x = adc_read();
        adc_select_input(JOYSTICK_Y_PIN - 26);
        uint16_t y = adc_read();
        bool btn = !gpio_get(JOYSTICK_BUTTON_PIN);

        // Atualiza display
        update_display(x, y, btn);
        printf("X: %4u | Y: %4u | BTN: %d\n", x, y, btn);

        sleep_ms(50);
    }
}
