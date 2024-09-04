/******************************************************************************
 * @file    ssd1306.c
 * @brief   Driver para o Display SSD1306 com ESP32
 * 
 * Este arquivo contém a implementação do driver para o display OLED SSD1306
 * utilizando o ESP32. Inclui funções para inicializar o display, configurar
 * parâmetros e desenhar gráficos e texto no display.
 * 
 * @version 1.0
 * @date    2024-09-03
 * 
 * @note    Certifique-se de configurar corretamente os pinos I2C e as configurações
 *          de inicialização de acordo com o seu hardware e especificações do display.
 * 
 * @copyright Copyright (c) 2024
 *           Todos os direitos reservados.
 ******************************************************************************
 */

#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"

#include "driver/i2c_master.h"

#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"

#include "ssd1306.h"

// Log tag para mensagens de debug e informações
static const char *TAG = "ssd1306";

// Handle do barramento I2C
static i2c_master_bus_handle_t i2c_bus = NULL;
// Handle do display LVGL
lv_disp_t *disp = NULL;

/**
 * @brief Desenha um bitmap no display utilizando a biblioteca LVGL.
 * 
 * Esta função bloqueia o mutex LVGL, cria uma imagem a partir do bitmap fornecido
 * e a centraliza na tela ativa do display.
 *
 * @param bitmap Ponteiro para o array de bytes que representa o bitmap.
 * @param disp Handle do display LVGL onde o bitmap será desenhado.
 */
void ssd1306_draw_bitmap(uint8_t *bitmap, lv_disp_t *disp)
{
    static lv_img_dsc_t img_dsc = 
    {
        .header.always_zero = 0,
        .header.w = SSD1306_H_RES,              
        .header.h = SSD1306_V_RES,                          
        .header.cf = LV_IMG_CF_ALPHA_1BIT,                           
    };

    img_dsc.data_size = SSD1306_H_RES * SSD1306_V_RES / 8;
    img_dsc.data = bitmap;

    ESP_LOGI(TAG, "Display LVGL Bitmap");

    // Bloqueia o mutex devido ao fato das APIs LVGL não serem thread-safe
    if (lvgl_port_lock(0)) 
    {
        lv_obj_t *scr = lv_disp_get_scr_act(disp); 
        lv_obj_t *img = lv_img_create(scr);       
        lv_img_set_src(img, &img_dsc);             
        lv_obj_center(img);

        // Libera o mutex
        lvgl_port_unlock();
    } 
}

/**
 * @brief Exibe um texto no display utilizando a biblioteca LVGL.
 * 
 * Esta função bloqueia o mutex LVGL, cria um rótulo de texto com uma
 * mensagem predefinida e a alinha no topo do display.
 *
 * @param disp Handle do display LVGL onde o texto será exibido.
 */
void ssd1306_print_text(lv_disp_t *disp)
{    
    // Bloqueia o mutex devido ao fato das APIs LVGL não serem thread-safe
    if (lvgl_port_lock(0)) 
    {
        lv_obj_t *scr = lv_disp_get_scr_act(disp);
        lv_obj_t *label = lv_label_create(scr);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_label_set_text(label, "Hello Espressif, Hello LVGL.");
        lv_obj_set_width(label, SSD1306_H_RES);
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        // Libera o mutex
        lvgl_port_unlock();
    } 
}

/**
 * @brief Inicializa o display OLED SSD1306 utilizando o driver LVGL.
 * 
 * Esta função configura o barramento I2C, inicializa o driver SSD1306 e a biblioteca LVGL.
 * Em seguida, adiciona a tela ao display LVGL e define a rotação da tela.
 */
void ssd1306_init(void)
{
    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = 
    {
        .dev_addr = SSD1306_I2C_HW_ADDR,
        .scl_speed_hz = SSD1306_PIXEL_CLOCK_HZ,
        .control_phase_bytes = 1,               
        .lcd_cmd_bits = SSD1306_CMD_BITS,      
        .lcd_param_bits = SSD1306_CMD_BITS, 
        .dc_bit_offset = 6,                     
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

    ESP_LOGI(TAG, "Install SSD1306 panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = 
    {
        .bits_per_pixel = 1,
        .reset_gpio_num = SSD1306_PIN_NUM_RST,
    };

    esp_lcd_panel_ssd1306_config_t ssd1306_config = 
    {
        .height = SSD1306_V_RES,
    };
    panel_config.vendor_config = &ssd1306_config;
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true)); 

    ESP_LOGI(TAG, "Initialize LVGL");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = 
    {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = SSD1306_H_RES * SSD1306_V_RES,
        .double_buffer = true,
        .hres = SSD1306_H_RES,
        .vres = SSD1306_V_RES,
        .monochrome = true,
        .rotation = 
        {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        }
    };
    disp = lvgl_port_add_disp(&disp_cfg); 

    lv_disp_set_rotation(disp, LV_DISP_ROT_NONE);
}

/**
 * @brief Inicializa o barramento I2C.
 * 
 * Esta função configura e inicializa o barramento I2C utilizando as definições
 * de hardware específicas para o display SSD1306.
 */
void i2c_init(void)
{
    ESP_LOGI(TAG, "Initialize I2C bus");
    i2c_master_bus_config_t bus_config = 
    {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_BUS_PORT,
        .sda_io_num = SSD1306_PIN_NUM_SDA,
        .scl_io_num = SSD1306_PIN_NUM_SCL,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));    
}

/**
 * @brief Inicia a configuração do display OLED SSD1306.
 * 
 * Esta função inicializa o periférico I2C e em seguida configura o display SSD1306
 * utilizando as funções de inicialização específicas do driver.
 */
void ssd1306_start(void)
{
    // Inicializa o periférico I2C
    i2c_init();

    // Inicializa as configurações do display OLED baseado no driver SSD1306
    ssd1306_init();
}