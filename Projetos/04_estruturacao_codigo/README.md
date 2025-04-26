# Projetos de Sistemas Embarcados - EmbarcaTech 2025

Autor: Pedro Henrique Tenorio de Magalhães de Oliveira

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Campinas, abril de 2025

---

## Sobre o programa
Estrutura Modular para Piscar o LED Embutido na Raspberry Pi Pico W
Este projeto demonstra como organizar um código para embarcados utilizando uma arquitetura modular,
separando as responsabilidades em diferentes camadas para facilitar manutenção e reutilização.

Estrutura de Diretórios

app/: Contém a lógica principal da aplicação (main.c).
hal/: Camada de abstração de hardware (HAL), expõe funções genéricas e reutilizáveis para controle do LED.
drivers/: Implementação do driver de acesso direto ao hardware, usando a API do SDK da Pico.
include/: Arquivos de cabeçalho (.h) compartilhados entre os módulos.

Funcionamento
O programa inicializa o LED embutido da Raspberry Pi Pico W e o faz piscar indefinidamente.
A aplicação (main.c) utiliza apenas funções da HAL, que por sua vez utiliza o driver para acessar o hardware.
