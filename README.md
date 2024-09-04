# Wearable para Auxílio de Surdos e Deficientes Auditivos na Identificação de Perigos Sonoros

## Descrição

Este projeto tem como objetivo a construção de um gadget vestível para auxiliar surdos e deficientes auditivos na identificação de perigos que podem ser detectados por sinais sonoros. O dispositivo é composto por:

- **Microcontrolador**
- **Microfone MEMS (Micro Eletromecânico)**
- **Motor de vibração**
- **Display OLED**

O dispositivo detecta sons ambientais e notifica o usuário sobre possíveis perigos através de vibrações e exibições no display OLED. A classificação dos sinais sonoros é feita em nuvem utilizando redes neurais, garantindo a melhor performance na identificação dos perigos.

## Funcionalidades

- **Captura de Áudio**: O microfone MEMS captura sons do ambiente.
- **Processamento e Envio**: O microcontrolador processa o sinal captado, monta um arquivo de áudio e o envia para o servidor na nuvem via WiFi.
- **Classificação de Sons**: O servidor na nuvem utiliza redes neurais para classificar os sons e identificar possíveis perigos.
- **Notificação ao Usuário**: O dispositivo sinaliza o perigo detectado através de vibrações e exibe a natureza do perigo no display OLED.

## Tecnologias Utilizadas

- **Microcontrolador**: ESP32-WROOM
- **Microfone MEMS**: INMP441
- **Display OLED**: SSD1306
- **Redes Neurais**: Algoritmos de classificação sonora executados em nuvem
- **Comunicação**: WiFi e protocolo HTTP/HTTPS

## Como Funciona

1. O microfone MEMS captura o som ambiente e o microcontrolador processa o sinal.
2. O sinal de áudio é enviado para um servidor na nuvem via WiFi.
3. O servidor na nuvem utiliza redes neurais para classificar o áudio e identificar perigos.
4. O microcontrolador recebe a resposta do servidor e notifica o usuário com vibrações e informações no display OLED.

## Instalação

1. **Clone o repositório:**
2. **Instale as dependências necessárias.**
3. **Configure o ambiente conforme as instruções no arquivo config.h.**
4. **Compile e faça o upload do código para o microcontrolador.**

## Contato
Para mais informações, entre em contato
**e-mail: phlangone@gmail.com**
**username: phlangone**




