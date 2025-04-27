# Projetos de Sistemas Embarcados - EmbarcaTech 2025

Autor: Pedro Henrique Tenorio de Magalhães de Oliveira

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Campinas, abril de 2025

---

## Sobre o programa

Este projeto implementa testes unitários em C utilizando o framework Unity para validar a conversão de valores de ADC (Conversor Analógico-Digital) em temperatura, conforme a documentação do sensor interno do microcontrolador RP2040 (Raspberry Pi Pico).

Os testes comparam os resultados da função de conversão para diferentes valores de ADC com referências calculadas pela fórmula oficial do datasheet, cobrindo valores mínimos, máximos e intermediários.

O projeto inclui:

Implementação da função de conversão de ADC para temperatura em graus Celsius.

Tabela de casos de teste com valores esperados.

Teste automatizado com Unity, que exibe os resultados em formato de tabela e valida a precisão da função.

Pausa ao final da execução para facilitar a visualização dos resultados ao rodar em ambiente gráfico.

