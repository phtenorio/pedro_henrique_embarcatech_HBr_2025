#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/timer.h"
#include "contador.h"

// Variáveis globais
volatile int contador = 0;
volatile int cliques_botao_b = 0;
volatile bool contagem_ativa = false;

// Implementação das funções declaradas no contador.h

void inicializar_gpio() {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
}

void inicializar_oled(SSD1306 *oled) {
    oled_init(oled, OLED_SDA_PIN, OLED_SCL_PIN);
}

void atualizar_display(SSD1306 *oled) {
    char buffer[32];
    oled_clear(oled);
    sprintf(buffer, "Contador: %d", contador);
    oled_draw_string(oled, 0, 0, buffer);
    sprintf(buffer, "Cliques B: %d", cliques_botao_b);
    oled_draw_string(oled, 0, 16, buffer);
    oled_show(oled);
}

bool timer_callback(struct repeating_timer *t) {
    if (contagem_ativa && contador > 0) {
        contador--;
        if (contador == 0) {
            contagem_ativa = false; // Congela o sistema ao atingir zero
        }
    }
    return true;
}

void gpio_callback_button_a(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A_PIN) {
        contador = 9;
        cliques_botao_b = 0;
        contagem_ativa = true;
    }
}

void gpio_callback_button_b(uint gpio, uint32_t events) {
    if (gpio == BUTTON_B_PIN && contagem_ativa) {
        cliques_botao_b++;
    }
}

int main() {
    // Inicialização do GPIO e OLED
    stdio_init_all();
    inicializar_gpio();

    SSD1306 oled;
    inicializar_oled(&oled);

    // Configuração de interrupções nos botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback_button_a);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback_button_b);

    // Configuração de timer repetitivo
    struct repeating_timer timer;
    add_repeating_timer_ms(-1000, timer_callback, NULL, &timer);

    while (true) {
        atualizar_display(&oled); // Atualiza o display OLED com os valores atuais
        sleep_ms(100); // Pequeno atraso para evitar sobrecarga na CPU
    }

    return 0;
}
