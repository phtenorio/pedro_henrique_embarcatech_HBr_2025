// Responsável pelo acesso direto ao hardware (API cyw43_arch)
#include "led_embutido.h"
#include "pico/cyw43_arch.h"

int led_embutido_init(void) {
    return cyw43_arch_init();
}

void led_embutido_set(int value) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, value);
}
