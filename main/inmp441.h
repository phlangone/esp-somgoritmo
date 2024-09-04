/******************************************************************************
 * @file    inmp441.h
 * @brief   Cabeçalho para a interface com o microfone INMP441.
 * 
 * Este arquivo define as constantes e funções para configurar e interagir
 * com o microfone INMP441 usando a interface I2S. Inclui definições de pinos
 * GPIO para o clock I2S e a entrada de dados, bem como configurações do buffer
 * e parâmetros de áudio.
 * 
 * @version 1.0
 * @date    2024-09-03
 * 
 * @note    Verifique as configurações de pinos GPIO e parâmetros de áudio para
 *          garantir que correspondam ao seu hardware e requisitos do projeto.
 * 
 * @copyright Copyright (c) 2024
 *           Todos os direitos reservados.
 ******************************************************************************
 */

#ifndef MAIN_INMP441_H_
#define MAIN_INMP441_H_

#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "esp_err.h"

// Pinos GPIO para I2S
#define INMP441_BCLK_GPIO        GPIO_NUM_4                     ///< Pino para o clock de bits I2S
#define INMP441_WS_GPIO          GPIO_NUM_5                     ///< Pino para a seleção de palavras I2S
#define INMP441_DIN_GPIO         GPIO_NUM_19                    ///< Pino para os dados de entrada I2S

#define INMP441_BUFF_SIZE            2048                       ///< Tamanho do buffer de áudio
#define INMP441_SAMPLE_RATE          44100                      ///< Taxa de amostragem do áudio
#define INMP441_BITS_PER_SAMPLE      I2S_DATA_BIT_WIDTH_16BIT   ///< Bits por amostra de áudio
#define INMP441_CHANNEL_MODE         I2S_SLOT_MODE_MONO         ///< Modo de canal do áudio

// Buffer para armazenar os dados lidos e variável para o número de bytes lidos
extern uint8_t *r_buf;
extern size_t r_bytes;

/**
 * @brief Inicializa o microfone INMP441 e a comunicação I2S, e inicia a task de leitura de áudio.
 * 
 * Esta função é chamada para configurar e iniciar a captura de áudio do microfone.
 */
void inmp441_start();

#endif /* MAIN_INMP441_H_ */
