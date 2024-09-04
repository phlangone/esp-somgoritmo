/******************************************************************************
 * @file    main.c
 * @brief   Ponto de entrada da aplicação.
 * 
 * Este arquivo contém a função principal `app_main`, que é o ponto de entrada
 * da aplicação. A função inicializa o sistema de arquivos NVS, configura a
 * conexão Wi-Fi, inicializa o periférico I2S e o microfone INMP441, e também
 * inicializa o display OLED SSD1306.
 * 
 * @version 1.0
 * @date    2024-09-03
 * 
 * @note    Certifique-se de que todas as dependências e configurações de hardware
 *          estão corretas antes de iniciar a aplicação. As funções `wifi_app_start`,
 *          `inmp441_start`, e `ssd1306_start` devem estar implementadas e configuradas
 *          adequadamente para garantir o funcionamento correto do sistema.
 * 
 * @copyright Copyright (c) 2024
 *           Todos os direitos reservados.
 ******************************************************************************
 */

#include "nvs_flash.h"

#include "wifi_app.h"
#include "inmp441.h"
#include "ssd1306.h"

void app_main(void)
{
    // Inicializa NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Inicializa Wi-Fi
    wifi_app_start();

    // Iniciliza o periférico I2S e o microfone INMP441
    inmp441_start();

    // Inicializa display OLED SSD1306
    ssd1306_start();
}