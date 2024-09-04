/******************************************************************************
 * @file    inmp441.c
 * @brief   Driver para Microfone INMP441 no ESP32
 * 
 * Este arquivo contém a implementação para inicialização e leitura de dados do
 * microfone INMP441 usando I2S no ESP32. Inclui funções para configurar a interface 
 * I2S e criar tarefas para lidar com dados de áudio.
 * 
 * @version 1.0
 * @date    2024-09-03
 * 
 * @note    Certifique-se de configurar corretamente os pinos I2S e GPIO de acordo
 *          com seu hardware e as especificações do microfone INMP441.
 * 
 * @copyright Copyright (c) 2024
 *           Todos os direitos reservados.
 ******************************************************************************
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"

#include "driver/i2s_std.h"
#include "driver/gpio.h"

#include "inmp441.h"
#include "tasks_common.h"

// Handler para o canal de recepção I2S
static i2s_chan_handle_t rx_chan;        

// Buffer para armazenar os dados lidos e variável para o número de bytes lidos
uint8_t *r_buf; 
size_t r_bytes;

// Tag usada para mensagens de log relacionadas ao INMP441
static const char *TAG = "inmp441";

/**
 * @brief Task responsável por ler os dados do microfone INMP441 através do canal I2S.
 * 
 * A leitura é feita continuamente, armazenando os dados no buffer `r_buf`.
 * Em caso de falha na leitura, uma mensagem de erro é registrada.
 * 
 * @param args Argumentos passados para a task (não utilizados neste caso).
 */
static void inmp441_read_task(void *args)
{
    ESP_LOGI(TAG, "Inicio da task de leitura do INMP441");

    // Aloca o buffer para armazenar os dados lidos do I2S
    r_buf = (uint8_t *)calloc(1, INMP441_BUFF_SIZE);
    assert(r_buf); // Verifica se a alocação do buffer foi bem-sucedida
    r_bytes = 0;

    // Habilita o canal de recepção I2S
    ESP_ERROR_CHECK(i2s_channel_enable(rx_chan));

    while (1) {
        // Lê os dados do canal I2S e armazena no buffer
        if (i2s_channel_read(rx_chan, r_buf, INMP441_BUFF_SIZE, &r_bytes, 1000) == ESP_OK) {
            // Se a leitura for bem-sucedida, o número de bytes lidos é armazenado em `r_bytes`
            //printf("Read Task: i2s read %d bytes\n-----------------------------------\n", r_bytes);
        } else {
            // Em caso de falha na leitura, registra uma mensagem de erro
            //printf("Read Task: i2s read failed\n");
        }
    }

    // Libera o buffer após o término da task
    free(r_buf);
    vTaskDelete(NULL);
}

/**
 * @brief Inicializa o canal I2S em modo simplex (apenas recepção) para o microfone INMP441.
 * Essa função configura o clock, slots e GPIOs necessários para a comunicação com o microfone.
 */
static void inmp441_init_simplex(void)
{
    // Passo 1: Determina a configuração do canal I2S e aloca um canal para recepção (RX)
    i2s_chan_config_t rx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&rx_chan_cfg, NULL, &rx_chan));

    // Passo 2: Configura as definições padrão do modo I2S e inicializa o canal
    i2s_std_config_t rx_std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(INMP441_SAMPLE_RATE),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(INMP441_BITS_PER_SAMPLE, INMP441_CHANNEL_MODE),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,    
            .bclk = INMP441_BCLK_GPIO,
            .ws   = INMP441_WS_GPIO,
            .dout = I2S_GPIO_UNUSED,
            .din  = INMP441_DIN_GPIO,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };

    // Configuração padrão: Recebe apenas o slot esquerdo em modo mono
    rx_std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT;
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_chan, &rx_std_cfg));
}

/**
 * @brief Inicializa o microfone INMP441 e a comunicação I2S, e inicia a task de leitura de áudio.
 * 
 * Esta função é chamada para configurar e iniciar a captura de áudio do microfone.
 */
void inmp441_start()
{
    // Inicializa o sensor e o periférico I2S em modo simplex
    inmp441_init_simplex();

    // Cria e inicia a task para gravação de áudio com o microfone INMP441
    xTaskCreatePinnedToCore(inmp441_read_task, "inmp441_read_task", INMP441_TASK_STACK_SIZE,
                                                                    NULL,
                                                                    INMP441_TASK_TASK_PRIORITY,
                                                                    NULL,
                                                                    INMP441_TASK_CORE_ID);
}
