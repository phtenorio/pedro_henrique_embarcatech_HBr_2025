#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "pico/rand.h"
#include "ssd1306.h"

#define I2C_PORT i2c1
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15
#define OLED_ADDR 0x3C

#define JOYSTICK_X_ADC 0 // ADC0 (GPIO26)
#define JOYSTICK_Y_ADC 1 // ADC1 (GPIO27)
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

#define JOY_UP_THRESHOLD     4030
#define JOY_DOWN_THRESHOLD   3800
#define JOY_RIGHT_THRESHOLD  4065
#define JOY_LEFT_THRESHOLD   3850

#define MOVING_AVERAGE_SIZE 10

#define LINHAS_MIN 1
#define LINHAS_MAX 100
#define BOLAS_MIN  2
#define BOLAS_MAX  10000

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define GRAFICO_BASE_Y 47   // base da barra (linha 47), para não invadir o rodapé
#define GRAFICO_ALTURA_MAX 32 // altura máxima da barra (deixa espaço para texto do botão e valor)
#define TEXTO_MENU_Y 56

typedef enum {JOY_NONE, JOY_UP, JOY_DOWN, JOY_LEFT, JOY_RIGHT} joy_dir_t;

uint16_t x_buffer[MOVING_AVERAGE_SIZE] = {0};
uint16_t y_buffer[MOVING_AVERAGE_SIZE] = {0};
int buffer_index = 0;

joy_dir_t read_joystick(uint16_t *x_avg_out, uint16_t *y_avg_out) {
    adc_select_input(JOYSTICK_X_ADC);
    uint16_t x_raw = adc_read();
    adc_select_input(JOYSTICK_Y_ADC);
    uint16_t y_raw = adc_read();

    x_buffer[buffer_index] = x_raw;
    y_buffer[buffer_index] = y_raw;
    buffer_index = (buffer_index + 1) % MOVING_AVERAGE_SIZE;

    uint32_t x_sum = 0, y_sum = 0;
    for (int i = 0; i < MOVING_AVERAGE_SIZE; i++) {
        x_sum += x_buffer[i];
        y_sum += y_buffer[i];
    }
    uint16_t x_avg = x_sum / MOVING_AVERAGE_SIZE;
    uint16_t y_avg = y_sum / MOVING_AVERAGE_SIZE;

    if (x_avg_out) *x_avg_out = x_avg;
    if (y_avg_out) *y_avg_out = y_avg;

    joy_dir_t dir = JOY_NONE;
    if (y_avg > JOY_UP_THRESHOLD) dir = JOY_UP;
    else if (y_avg < JOY_DOWN_THRESHOLD) dir = JOY_DOWN;
    else if (x_avg > JOY_RIGHT_THRESHOLD) dir = JOY_RIGHT;
    else if (x_avg < JOY_LEFT_THRESHOLD) dir = JOY_LEFT;

    return dir;
}

void show_config_screen(ssd1306_t *oled, int linhas, int bolas, int sel) {
    char buf[32];
    ssd1306_clear(oled);

    ssd1306_draw_string(oled, 0, 0, 1, "Configurar Galton:");
    snprintf(buf, sizeof(buf), "%s Linhas: %d", (sel==0)?"->":"  ", linhas);
    ssd1306_draw_string(oled, 0, 16, 1, buf);
    snprintf(buf, sizeof(buf), "%s Bolas:  %d", (sel==1)?"->":"  ", bolas);
    ssd1306_draw_string(oled, 0, 32, 1, buf);
    ssd1306_draw_string(oled, 0, 48, 1, "Pressione Botao A");
    ssd1306_show(oled);
}

// Gráfico de barras com ajuste automático e paginação, sem sobrescrever rodapé
void show_bar_graph(ssd1306_t *oled, int *vetor, int num_linhas, int pagina, int max_por_pagina, int total_paginas) {
    int tamanho_vetor = 2 * num_linhas + 1;
    int valores_possiveis[LINHAS_MAX+1];
    int num_canaletas = 0;

    // Filtra apenas as canaletas possíveis
    for (int i = 0; i < tamanho_vetor; i++) {
        int posicao = i - num_linhas;
        if ((num_linhas + posicao) % 2 == 0) {
            valores_possiveis[num_canaletas++] = vetor[i];
        }
    }

    // Paginação
    int inicio = pagina * max_por_pagina;
    int fim = inicio + max_por_pagina;
    if (fim > num_canaletas) fim = num_canaletas;
    int barras_na_pagina = fim - inicio;

    // Descobre o maior valor para normalizar as barras
    int max_val = 1;
    for (int i = inicio; i < fim; i++)
        if (valores_possiveis[i] > max_val) max_val = valores_possiveis[i];

    ssd1306_clear(oled);

    // Ajuste automático da largura
    int espacamento = 1;
    int largura = (OLED_WIDTH - (barras_na_pagina + 1) * espacamento) / barras_na_pagina;
    if (largura < 2) largura = 2;
    int base_y = GRAFICO_BASE_Y;
    int altura_max = GRAFICO_ALTURA_MAX;

    for (int i = inicio; i < fim; i++) {
        int h = (valores_possiveis[i] * altura_max) / max_val;
        int barra_idx = i - inicio;
        int x = barra_idx * (largura + espacamento) + espacamento;
        // Desenha barra vertical preenchida
        for (int dx = 0; dx < largura; dx++) {
            for (int dy = 0; dy < h; dy++) {
                ssd1306_draw_pixel(oled, x + dx, base_y - dy);
            }
        }
        // Desenha o valor acima da barra
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", valores_possiveis[i]);
        ssd1306_draw_string(oled, x, base_y - h - 10, 1, buf);

        // Opcional: desenha o índice da canaleta abaixo da barra
        char idxbuf[4];
        snprintf(idxbuf, sizeof(idxbuf), "%d", i);
        ssd1306_draw_string(oled, x, base_y + 2, 1, idxbuf);
    }

    // Rodapé com botões A e B
    ssd1306_draw_string(oled, 0, TEXTO_MENU_Y, 1, "A: menu  B: next");

    // Paginação: indica página atual se houver mais de 1 página
    if (total_paginas > 1) {
        char buf[16];
        snprintf(buf, sizeof(buf), "Pag %d/%d", pagina+1, total_paginas);
        ssd1306_draw_string(oled, OLED_WIDTH - 60, 0, 1, buf);
    }

    ssd1306_show(oled);
}

int main()
{
    stdio_init_all();

    // Inicializa I2C e OLED
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    ssd1306_t oled;
    ssd1306_init(&oled, 128, 64, OLED_ADDR, I2C_PORT);

    // Inicializa entradas do joystick
    adc_init();
    adc_gpio_init(26); // GPIO26 para ADC0 (X)
    adc_gpio_init(27); // GPIO27 para ADC1 (Y)

    // Inicializa Botão A e B
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    int num_linhas = 3;
    int num_bolas = 1000;
    int selecionado = 0; // 0: linhas, 1: bolas

    enum {STATE_CONFIG, STATE_RUN, STATE_RESULT} state = STATE_CONFIG;

    int *vetor = NULL;
    int tamanho_vetor = 0;

    // Variáveis para controle do tempo e direção
    absolute_time_t joy_time = get_absolute_time();
    joy_dir_t last_dir = JOY_NONE;
    int joy_hold_count = 0;
    bool can_change_selection = true;

    // Para paginação no gráfico
    int pagina_atual = 0;
    int max_barras_por_pagina = 8; // Ajuste para caber barras largas/legíveis

    while (1)
    {
        if (state == STATE_CONFIG) {
            show_config_screen(&oled, num_linhas, num_bolas, selecionado);

            bool mudou = false;
            while (1) {
                uint16_t x_avg, y_avg;
                joy_dir_t dir = read_joystick(&x_avg, &y_avg);

                // Para seleção (LEFT/RIGHT): só permite trocar se voltou para NONE antes
                if ((dir == JOY_LEFT || dir == JOY_RIGHT) && can_change_selection) {
                    selecionado = 1 - selecionado;
                    mudou = true;
                    can_change_selection = false;
                    show_config_screen(&oled, num_linhas, num_bolas, selecionado);
                }
                if (dir == JOY_NONE) {
                    can_change_selection = true;
                }

                // Para UP/DOWN: altera linhas/bolas com aceleração diferenciada
                if (dir == JOY_UP || dir == JOY_DOWN) {
                    if (dir != last_dir) {
                        joy_time = get_absolute_time();
                        joy_hold_count = 0;
                    }
                    int delta = 0;
                    int elapsed = absolute_time_diff_us(joy_time, get_absolute_time()) / 1000000;
                    if (elapsed >= 1) {
                        joy_hold_count++;
                        joy_time = get_absolute_time();
                        if (selecionado == 0) {
                            delta = (joy_hold_count < 3) ? 1 : 2;
                        } else {
                            if (joy_hold_count < 3) delta = 1;
                            else if (joy_hold_count < 6) delta = 10;
                            else delta = 100;
                        }
                        if (dir == JOY_UP) {
                            if (selecionado == 0) {
                                num_linhas += delta;
                                if (num_linhas > LINHAS_MAX) num_linhas = LINHAS_MAX;
                            } else {
                                num_bolas += delta;
                                if (num_bolas > BOLAS_MAX) num_bolas = BOLAS_MAX;
                            }
                        } else if (dir == JOY_DOWN) {
                            if (selecionado == 0) {
                                num_linhas -= delta;
                                if (num_linhas < LINHAS_MIN) num_linhas = LINHAS_MIN;
                            } else {
                                num_bolas -= delta;
                                if (num_bolas < BOLAS_MIN) num_bolas = BOLAS_MIN;
                            }
                        }
                        mudou = true;
                    }
                    last_dir = dir;
                } else if (dir != JOY_LEFT && dir != JOY_RIGHT) {
                    last_dir = dir;
                    joy_hold_count = 0;
                    joy_time = get_absolute_time();
                }

                if (mudou) {
                    show_config_screen(&oled, num_linhas, num_bolas, selecionado);
                    mudou = false;
                }

                if (gpio_get(BUTTON_A_PIN) == 0) {
                    while (gpio_get(BUTTON_A_PIN) == 0) sleep_ms(10);
                    state = STATE_RUN;
                    break;
                }
                sleep_ms(5);
            }
        }

        if (state == STATE_RUN) {
            tamanho_vetor = 2 * num_linhas + 1;
            if (vetor) free(vetor);
            vetor = (int *)calloc(tamanho_vetor, sizeof(int));
            if (vetor == NULL) {
                ssd1306_clear(&oled);
                ssd1306_draw_string(&oled, 0, 0, 1, "Erro memoria!");
                ssd1306_show(&oled);
                sleep_ms(2000);
                return 1;
            }

            ssd1306_clear(&oled);
            ssd1306_draw_string(&oled, 0, 0, 1, "Executando...");
            ssd1306_show(&oled);

            for (int j = 0; j < num_bolas; j++) {
                int posicao = 0;
                for (int i = 0; i < num_linhas; i++) {
                    posicao += (get_rand_32() & 1) ? 1 : -1;
                }
                vetor[posicao + num_linhas]++;
            }
            pagina_atual = 0;
            state = STATE_RESULT;
        }

        if (state == STATE_RESULT) {
            // Paginação: calcula quantas barras por página cabem
            int tamanho_vetor = 2 * num_linhas + 1;
            int num_canaletas = 0;
            for (int i = 0; i < tamanho_vetor; i++) {
                int posicao = i - num_linhas;
                if ((num_linhas + posicao) % 2 == 0) num_canaletas++;
            }
            int total_paginas = (num_canaletas + max_barras_por_pagina - 1) / max_barras_por_pagina;

            show_bar_graph(&oled, vetor, num_linhas, pagina_atual, max_barras_por_pagina, total_paginas);

            // Paginação via botão B
            bool aguardando = true;
            while (aguardando) {
                uint16_t x_avg, y_avg;
                joy_dir_t dir = read_joystick(&x_avg, &y_avg);

                if (gpio_get(BUTTON_B_PIN) == 0) {
                    // Próxima página
                    while (gpio_get(BUTTON_B_PIN) == 0) sleep_ms(10);
                    pagina_atual++;
                    if (pagina_atual >= total_paginas) pagina_atual = 0;
                    show_bar_graph(&oled, vetor, num_linhas, pagina_atual, max_barras_por_pagina, total_paginas);
                }
                if (gpio_get(BUTTON_A_PIN) == 0) {
                    while (gpio_get(BUTTON_A_PIN) == 0) sleep_ms(10);
                    aguardando = false;
                    state = STATE_CONFIG;
                }
                sleep_ms(20);
            }
        }
    }

    if (vetor) free(vetor);
    return 0;
}
