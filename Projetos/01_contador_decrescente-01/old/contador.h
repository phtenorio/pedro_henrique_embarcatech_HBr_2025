#ifndef CONTADOR_H
#define CONTADOR_H

#include "ssd1306.h" // Biblioteca para o display OLED

// Definição dos pinos
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define OLED_SDA_PIN 14
#define OLED_SCL_PIN 15

// Funções para inicialização
void inicializar_gpio();
void inicializar_oled(SSD1306 *oled);

// Funções para interrupções
void gpio_callback_button_a(uint gpio, uint32_t events);
void gpio_callback_button_b(uint gpio, uint32_t events);

// Função para atualizar o display OLED
void atualizar_display(SSD1306 *oled);

// Função de callback do timer
bool timer_callback(struct repeating_timer *t);

#endif // CONTADOR_H
