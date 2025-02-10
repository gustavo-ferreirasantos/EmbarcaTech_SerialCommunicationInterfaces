# EmbarcaTech_SerialCommunicationInterfaces

## 📌 DESCRIÇÃO
Este projeto tem como objetivo consolidar os conceitos sobre o uso de interfaces de comunicação serial no microcontrolador RP2040, além de explorar as funcionalidades da placa de desenvolvimento BitDogLab. O projeto envolve a comunicação via UART e I2C, o controle de LEDs comuns e endereçáveis WS2812, a interação com botões físicos e o uso de interrupções para tratamento de eventos.

---

## 🎯 OBJETIVOS
✅ Compreender o funcionamento e a aplicação de comunicação serial em microcontroladores.  
✅ Aplicar os conhecimentos adquiridos sobre UART e I2C na prática.  
✅ Manipular e controlar LEDs comuns e LEDs endereçáveis WS2812.  
✅ Fixar o estudo do uso botões de acionamento, interrupções e Debounce.  
✅ Desenvolver um projeto funcional que combine hardware e software.  

---

## 📋 FUNCIONALIDADES
🔹 Modificação da biblioteca font.h para inclusão de caracteres minúsculos.  
🔹 Entrada de caracteres via Serial Monitor do VS Code, exibindo-os no display SSD1306.  
🔹 Exibição de símbolos na matriz de LEDs WS2812 ao digitar números de 0 a 9.  
🔹 Interação com o botão A para alternar o estado do LED RGB Verde, com atualização no display e no Serial Monitor.  
🔹 Interação com o botão B para alternar o estado do LED RGB Azul, com atualização no display e no Serial Monitor.  
🔹 Implementação de interrupções (IRQ) para controle dos botões.  
🔹 Implementação de debouncing para evitar leituras falsas dos botões.  

---

## 🛠️ PRÉ-REQUISITOS

- 🛠️ HARDWARE NECESSÁRIO:
  
  - Placa de desenvolvimento BitDogLab (RP2040).
  - Microcontrolador Raspberry Pi Pico W.
  - Matriz 5x5 de LEDs (endereçáveis) WS2812, conectada à GPIO 7.
  - LED RGB, com os pinos conectados às GPIOs (11, 12 e 13).
  - Botão A conectado à GPIO 5.
  - Botão B conectado à GPIO 6.
  - Display SSD1306 conectado via I2C (GPIO 14 e GPIO15).
  
- 🖥 SOFTWARE NECESSÁRIO:
  - Raspberry Pi Pico SDK configurado.
  - CMake para compilação.
  - VS Code com a extensão Raspberry Pi Pico.
  - PuTTY ou Serial Monitor do VS Code para comunicação UART

---

## 🔧 REQUISITOS DO PROJETO  
✅ Uso de interrupções: Todas as funcionalidades relacionadas aos botões devem ser implementadas utilizando rotinas de interrupção (IRQ).  
✅ Debouncing: É obrigatório implementar o tratamento do bouncing dos botões via software.  
✅ Controle de LEDs: O projeto deve incluir o uso de LEDs comuns e LEDs WS2812, demonstrando o domínio de diferentes tipos de controle.  
✅ Utilização do Display 128 x 64: A utilização de fontes maiúsculas e minúsculas demonstrará o domínio do uso de bibliotecas, o entendimento do princípio de funcionamento do display, bem como, a utilização do protocolo I2C.  
✅ Envio de informação pela UART: Visa observar a compreensão sobre a comunicação serial via UART.  
✅ Organização do código: O código deve estar bem estruturado e comentado para facilitar o entendimento.  

---

## 🚀 COMO EXECUTAR

1️⃣ **Clone este repositório:**

       git clone https://github.com/gustavo-ferreirasantos/EmbarcaTech_SerialCommunicationInterfaces.git

2️⃣ Abra o projeto no VS Code e importe.

3️⃣ Compile e carregue o código na BitDogLab usando o SDK do Raspberry Pi Pico.

4️⃣ Conecte-se ao Serial Monitor para enviar caracteres e visualizar as mensagens.

---

## 🎥 Vídeo de demonstração
🔗 <u>[Assista aqui](https://youtu.be/pNvFfSHKf5E)</u>

