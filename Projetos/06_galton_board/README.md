# Projetos de Sistemas Embarcados - EmbarcaTech 2025

Autor: Pedro Henrique Tenorio de Magalhães de Oliveira

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Campinas, maio de 2025

---

## Sobre o programa

Galton Board Interativa no Raspberry Pi Pico W

Este projeto implementa uma simulação visual da Galton Board (também conhecida como Máquina de Galton ou Quincunx) usando um Raspberry Pi Pico W, um display OLED SSD1306 e um joystick analógico.

Funcionalidades
Configuração interativa:
Use o joystick para escolher o número de linhas e de bolas da simulação.

Aceleração inteligente:
Aumente/abaixe rapidamente os valores mantendo o joystick pressionado.

Execução da simulação:
O Pico W simula o caminho de cada bola pela Galton Board, distribuindo-as nas canaletas finais.

Visualização gráfica:
O resultado é exibido como um gráfico de barras no display OLED, mostrando a distribuição das bolas nas canaletas.

Paginação:
Se houver mais canaletas do que cabem na tela, use o Botão B para navegar entre as páginas do gráfico.

Controles intuitivos:

Joystick cima/baixo: alterna entre linhas e bolas na configuração.
Joystick esquerda/direita: aumenta/diminui linhas ou bolas.
Botão A: inicia a simulação e retorna ao menu.
Botão B: avança para a próxima página do gráfico de barras.

Sobre a Galton Board
A Galton Board é um dispositivo clássico de probabilidade que demonstra a distribuição normal (curva de Gauss) a partir de eventos binários aleatórios. Cada bola, ao cair, toma decisões aleatórias (esquerda/direita) ao passar por cada linha de pinos, resultando em uma distribuição característica nas canaletas inferiores.

