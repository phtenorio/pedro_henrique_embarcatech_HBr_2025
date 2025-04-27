#include "unity.h"
#include "adc_to_celsius.h"
#ifdef _WIN32
#include <stdlib.h>
#endif


typedef struct {
    int adc_val;
    float temp_ref;
} TestCase;

TestCase tabela[] = {
    {    0,  437.22f },
    {  512,  197.48f },
    {  876,   27.03f }, // 27°C
    { 2048, -521.75f },
    { 4095, -1480.26f }
};

void setUp(void) {}
void tearDown(void) {}

void test_adc_to_celsius_tabela(void) {
    printf("ADC Value | Temperatura Esperada | Temperatura Funcao\n");
    printf("-----------------------------------------------------\n");
    for (int i = 0; i < 5; i++) {
        float resultado = adc_to_celsius(tabela[i].adc_val);
        printf("%8d | %18.2f | %18.4f\n", tabela[i].adc_val, tabela[i].temp_ref, resultado);

        char msg[100];
        snprintf(msg, sizeof(msg), "Falha no caso %d (ADC=%d)", i, tabela[i].adc_val);
        TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.01f, tabela[i].temp_ref, resultado, msg);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_adc_to_celsius_tabela);
    UNITY_END();

#ifdef _WIN32
    system("pause");
#else
    getchar();
    getchar();
#endif
    return 0;
}
