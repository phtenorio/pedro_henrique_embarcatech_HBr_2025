// =========================== Includes ==========================
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

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

// ========================= Funções ==========================

// === Callback para finalizar debounce do botão A ===
int64_t debounce_timer_callback_a(alarm_id_t id, void *user_data) {
    if (!gpio_get(BUTTON_A)) { // Verifica se o botão ainda está pressionado
        button_a_ready = true; // Marca que o botão A está pronto para ser processado
    }
    debouncing_a = false;      // Marca que o debounce terminou
    return 0;
}

// === Callback para finalizar debounce do botão B ===
int64_t debounce_timer_callback_b(alarm_id_t id, void *user_data) {
    if (!gpio_get(BUTTON_B)) { // Verifica se o botão ainda está pressionado
        button_b_ready = true; // Marca que o botão B está pronto para ser processado
    }
    debouncing_b = false;      // Marca que o debounce terminou
    return 0;
}

// === Callback para interrupção dos botões A e B ===
void button_isr(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A && !debouncing_a) {
        debouncing_a = true;   // Inicia o estado de debounce para A
        add_alarm_in_ms(DEBOUNCE_TIME_MS, debounce_timer_callback_a, NULL, false);
    }
    else if (gpio == BUTTON_B && !debouncing_b) {
        debouncing_b = true;   // Inicia o estado de debounce para B
        add_alarm_in_ms(DEBOUNCE_TIME_MS, debounce_timer_callback_b, NULL, false);
    }
}

// === Callback do timer para decrementar o contador ===
bool timer_callback(repeating_timer_t *rt) {
    passed_1s = true;              // Sinaliza que 1 segundo passou
    return true;
}

// ===================== Programa Principal =====================
int main() {
    stdio_init_all();              // Inicializa comunicação via USB serial

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

    while (true) {
        // Verifica se o botão A está pronto após debounce e ainda está pressionado
        if (button_a_ready) {
            button_a_ready = false;

            if (countdown == 0) {
                button_b_count = 0;  // Zera a contagem de cliques do Botão B se contador estava em 0
            }
            countdown = 9;          // Reinicia o contador para 9
        }

        // Verifica se o botão B está pronto após debounce e ainda está pressionado
        if (button_b_ready) {
            button_b_ready = false;

            if (countdown > 0) {
                button_b_count++;   // Incrementa a contagem de cliques do Botão B se contador > 0
            }
        }

        // Verifica se passou 1 segundo para decrementar o contador regressivo
        if (passed_1s) {
            passed_1s = false;

            if (countdown > 0) {
                countdown--;        // Decrementa o contador se ainda for maior que zero
            }
        }

        printf("Contador: %d | Cliques Botão B: %d\n", countdown, button_b_count);

        // sleep_ms(100);              // Pequeno atraso para evitar sobrecarga no console serial
    }

    return 0;
}
