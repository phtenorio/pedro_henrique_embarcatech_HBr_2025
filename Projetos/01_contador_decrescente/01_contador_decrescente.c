// =========================== Includes ==========================
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <string.h>
#include "hardware/i2c.h"
#include "ssd1306_i2c.h"
#include "ssd1306.h"

// ====================== Variáveis Globais ======================
#define BUTTON_A 5 // Botão A
#define BUTTON_B 6 // Botão B
#define DEBOUNCE_TIME_MS 100 // Tempo de debounce em milissegundos

volatile int countdown = 9;       // Valor do contador regressivo
volatile int button_b_count = 0;  // Contagem de pressionamentos do Botão B

volatile bool debouncing_a = false; // Estado de debounce do botão A
volatile bool debouncing_b = false; // Estado de debounce do botão B
volatile bool passed_1s = false;    // Indica que 1 segundo passou

volatile bool button_a_ready = false; // Indica que o botão A está pronto para ser processado
volatile bool button_b_ready = false; // Indica que o botão B está pronto para ser processado

// Configuração do display OLED
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define I2C_PORT i2c1
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15
#define OLED_I2C_ADDRESS 0x3C

ssd1306_t oled;

// ========================= Funções ==========================

// === Callback para finalizar debounce do botão A ===
int64_t debounce_timer_callback_a(alarm_id_t id, void *user_data) {
    if (!gpio_get(BUTTON_A)) {
        button_a_ready = true;
    }
    debouncing_a = false;
    return 0;
}

// === Callback para finalizar debounce do botão B ===
int64_t debounce_timer_callback_b(alarm_id_t id, void *user_data) {
    if (!gpio_get(BUTTON_B)) {
        button_b_ready = true;
    }
    debouncing_b = false;
    return 0;
}

// === Callback para interrupção dos botões A e B ===
void button_isr(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A && !debouncing_a) {
        debouncing_a = true;
        add_alarm_in_ms(DEBOUNCE_TIME_MS, debounce_timer_callback_a, NULL, false);
    } else if (gpio == BUTTON_B && !debouncing_b) {
        debouncing_b = true;
        add_alarm_in_ms(DEBOUNCE_TIME_MS, debounce_timer_callback_b, NULL, false);
    }
}

// === Callback do timer para decrementar o contador ===
bool timer_callback(repeating_timer_t *rt) {
    passed_1s = true;
    return true;
}

// Função para inicializar o display OLED
void init_oled() {
    i2c_init(I2C_PORT, ssd1306_i2c_clock * 1000); // Inicializa I2C com frequência de 400kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);
}

// Função para atualizar o display com os valores atuais
void update_oled_display() {
    // Define a área de renderização para todo o display
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // Zera o buffer do display antes de desenhar novos dados
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    // Desenha as informações no buffer
    char buffer[32];
    
    snprintf(buffer, sizeof(buffer), "Contador: %d", countdown);
    ssd1306_draw_string(ssd, 0, 0, buffer);

    snprintf(buffer, sizeof(buffer), "Cliques B: %d", button_b_count);
    ssd1306_draw_string(ssd, 0, 16, buffer);

    // Renderiza o buffer no display
    render_on_display(ssd, &frame_area);
}

// ===================== Programa Principal =====================
int main() {
    stdio_init_all();

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_isr);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_isr);

    repeating_timer_t timer;
    add_repeating_timer_ms(1000, timer_callback, NULL, &timer);

    init_oled(); // Inicializa o display OLED

    while (true) {
        if (button_a_ready) {
            button_a_ready = false;
            button_b_count = 0;
            countdown = 9;
        }

        if (button_b_ready) {
            button_b_ready = false;

            if (countdown > 0) {
                button_b_count++;
            }
        }

        if (passed_1s) {
            passed_1s = false;

            if (countdown > 0) {
                countdown--;
            }
        }

        update_oled_display(); // Atualiza o display com os valores atuais

        sleep_ms(100); // Pequeno atraso para evitar sobrecarga no loop principal
    }

    return 0;
}
