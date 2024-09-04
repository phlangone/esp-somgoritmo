/******************************************************************************
 * @file    ssd1306.h
 * @brief   Cabeçalho para a interface com o display OLED SSD1306.
 * 
 * Este arquivo define as constantes e funções para configurar e interagir
 * com o display OLED SSD1306 usando a biblioteca LVGL. Inclui configurações
 * de hardware, como pinos e endereços I2C, e funções para desenhar bitmaps
 * e exibir texto no display.
 * 
 * @version 1.0
 * @date    2024-09-03
 * 
 * @note    Verifique as configurações de pinos e endereços para garantir que
 *          correspondam ao seu hardware. Ajuste a resolução e os parâmetros
 *          conforme necessário para o seu projeto.
 * 
 * @copyright Copyright (c) 2024
 *           Todos os direitos reservados.
 ******************************************************************************
 */

#ifndef MAIN_SSD1366_H_
#define MAIN_SSD1366_H_

#include "lvgl.h"

// Configurações do display SSD1306
#define SSD1306_PIXEL_CLOCK_HZ         (400 * 1000)    ///< Frequência do clock de pixel em Hz
#define SSD1306_PIN_NUM_SDA            21              ///< Pino SDA do barramento I2C
#define SSD1306_PIN_NUM_SCL            22              ///< Pino SCL do barramento I2C
#define SSD1306_PIN_NUM_RST            -1              ///< Pino de reset (não utilizado)
#define SSD1306_I2C_HW_ADDR            0x3C            ///< Endereço I2C do display SSD1306
#define SSD1306_H_RES                  128             ///< Resolução horizontal do display
#define SSD1306_V_RES                  64              ///< Resolução vertical do display
#define SSD1306_CMD_BITS               8               ///< Bits de comando I2C
#define SSD1306_PARAM_BITS             8               ///< Bits de parâmetro I2C
#define I2C_BUS_PORT                   1               ///< Porta do barramento I2C

extern lv_disp_t *disp; ///< Referência para o display LVGL

/**
 * @brief Desenha um bitmap no display utilizando a biblioteca LVGL.
 * 
 * Esta função bloqueia o mutex LVGL, cria uma imagem a partir do bitmap fornecido
 * e a centraliza na tela ativa do display.
 *
 * @param bitmap Ponteiro para o array de bytes que representa o bitmap.
 * @param disp Handle do display LVGL onde o bitmap será desenhado.
 */
void ssd1306_draw_bitmap(uint8_t *bitmap, lv_disp_t *disp);

/**
 * @brief Exibe um texto no display utilizando a biblioteca LVGL.
 * 
 * Esta função bloqueia o mutex LVGL, cria um rótulo de texto com uma
 * mensagem predefinida e a alinha no topo do display.
 *
 * @param disp Handle do display LVGL onde o texto será exibido.
 */
void ssd1306_print_text(lv_disp_t *disp);

/**
 * @brief Inicia a configuração do display OLED SSD1306.
 * 
 * Esta função inicializa o periférico I2C e em seguida configura o display SSD1306
 * utilizando as funções de inicialização específicas do driver.
 */
void ssd1306_start(void);

#endif /* MAIN_SSD1366_H_ */
