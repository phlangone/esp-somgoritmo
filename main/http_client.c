/******************************************************************************
 * @file    http_client.c
 * @brief   Cliente HTTP para ESP32
 * 
 * Este arquivo contém a implementação de um cliente HTTP para o ESP32, 
 * incluindo funções para fazer requisições POST com codificação em chunks. 
 * Também inclui o gerenciamento de eventos para respostas HTTP e a criação 
 * de tarefas para requisições HTTP periódicas.
 * 
 * @version 1.0
 * @date    2024-09-03
 * 
 * @note    Certifique-se de definir as macros necessárias, como HTTP_SERVER_URL,
 *          e configurar as definições do cliente HTTP de acordo com os requisitos 
 *          do seu servidor.
 * 
 * @copyright Copyright (c) 2024
 *           Todos os direitos reservados.
 ******************************************************************************
 */

#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>

#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_http_client.h"

#include "inmp441.h"
#include "tasks_common.h"
#include "wifi_app.h"
#include "http_client.h"

// Definições de tamanho de buffers
#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
#define STREAM_BUFFER_SIZE INMP441_SAMPLE_RATE * (INMP441_BITS_PER_SAMPLE / 8) * INMP441_CHANNEL_MODE

// Habilita mensagens de depuração (0 = desativado, 1 = ativado)
#define DEBUG 0

static const char *TAG = "http_client";

/**
 * Callback para eventos HTTP.
 * Lida com diferentes eventos durante o ciclo de vida de uma requisição HTTP.
 * 
 * @param evt Estrutura que contém informações sobre o evento.
 * @return esp_err_t Indica o status de execução do evento.
 */
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer para armazenar a resposta da requisição HTTP
    static int output_len;       // Armazena o número de bytes lidos

    switch(evt->event_id) 
    {
        case HTTP_EVENT_ERROR:
        {
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        }
        case HTTP_EVENT_ON_CONNECTED:
        {
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        }
        case HTTP_EVENT_HEADER_SENT:
        {
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        }
        case HTTP_EVENT_ON_HEADER:
        {
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        }
        case HTTP_EVENT_ON_DATA:
        {
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

            // Limpa o buffer se for uma nova requisição
            if (output_len == 0 && evt->user_data) 
            {
                memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
            }

            // Verifica se a resposta não está em chunked encoding
            if (!esp_http_client_is_chunked_response(evt->client)) 
            {
                int copy_len = 0;
                if (evt->user_data) 
                {
                    // Copia a resposta para o buffer de user_data
                    copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                    if (copy_len) 
                    {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } 
                else 
                {
                    // Copia a resposta para o buffer interno
                    int content_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) 
                    {
                        output_buffer = (char *) calloc(content_len + 1, sizeof(char));
                        output_len = 0;
                        if (output_buffer == NULL) 
                        {
                            ESP_LOGE(TAG, "Falha ao alocar memória para o buffer de saída");
                            return ESP_FAIL;
                        }
                    }
                    copy_len = MIN(evt->data_len, (content_len - output_len));
                    if (copy_len) 
                    {
                        memcpy(output_buffer + output_len, evt->data, copy_len);
                    }
                }
                output_len += copy_len;
            }

            break;
        }
        case HTTP_EVENT_ON_FINISH:
        {
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) 
            {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        }
        case HTTP_EVENT_DISCONNECTED:
        {
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
            if (err != 0) 
            {
                ESP_LOGI(TAG, "Último código de erro esp: 0x%x", err);
                ESP_LOGI(TAG, "Último erro mbedtls: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) 
            {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        }
        case HTTP_EVENT_REDIRECT:
        {
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            esp_http_client_set_redirection(evt->client);
            break;
        }
    }
    return ESP_OK;
}

/**
 * Função responsável por realizar uma requisição HTTP POST com chunked transfer.
 * O áudio capturado do microfone é enviado em blocos (chunks) para o servidor HTTP.
 * 
 * @return esp_err_t Indica o status de execução da requisição HTTP.
 */
static esp_err_t http_post(void)
{
    // Buffer local para armazenar a resposta da requisição HTTP
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};
    
    // Configuração do cliente HTTP
    esp_http_client_config_t config = {
        .url = HTTP_SERVER_URL,
        .method = HTTP_METHOD_POST,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,        
        .disable_auto_redirect = true,
        .timeout_ms = 10000
    };

    // Inicializa o cliente HTTP
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Define o método HTTP como POST
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Transfer-Encoding", "chunked");
    
    // Define os cabeçalhos HTTP personalizados para enviar metadados de áudio
    char dat[10] = {0};
    snprintf(dat, sizeof(dat), "%d", INMP441_SAMPLE_RATE);
    esp_http_client_set_header(client, "x-audio-sample-rates", dat);
    
    memset(dat, 0, sizeof(dat));
    snprintf(dat, sizeof(dat), "%d", INMP441_BITS_PER_SAMPLE);
    esp_http_client_set_header(client, "x-audio-bits", dat);
    
    memset(dat, 0, sizeof(dat));
    snprintf(dat, sizeof(dat), "%d", INMP441_CHANNEL_MODE);
    esp_http_client_set_header(client, "x-audio-channel", dat);

    // Abre a conexão HTTP
    esp_http_client_open(client, 0);

    // Calcula o tamanho do buffer de áudio
    static uint8_t audio_buffer[STREAM_BUFFER_SIZE] = {0};

    for (int i = 0; i < 4; i++) 
    {
        size_t offset = 0;

        // Preenchimento do buffer com dados de áudio
        while (offset < STREAM_BUFFER_SIZE) 
        {
            // Ajusta o tamanho de r_bytes para evitar overflow do buffer
            size_t bytes_to_copy = r_bytes;
            if (offset + r_bytes > STREAM_BUFFER_SIZE) 
            {
                bytes_to_copy = STREAM_BUFFER_SIZE - offset;  // Ajusta para evitar estouro
                if(DEBUG)
                {
                    ESP_LOGW(TAG, "Tamanho de cópia ajustado para evitar estouro: %d bytes", bytes_to_copy);
                }
            }

            // Copia os dados de r_buf para o buffer de áudio
            memcpy(audio_buffer + offset, r_buf, bytes_to_copy);
            offset += bytes_to_copy;

            // Log de depuração para acompanhar o progresso
            if(DEBUG) 
            {
                ESP_LOGI(TAG, "Copiados %d bytes, offset atual: %d", bytes_to_copy, offset);
            }

            // Se o buffer estiver cheio, sai do loop
            if (offset >= STREAM_BUFFER_SIZE) 
            {
                if(DEBUG)
                {
                    ESP_LOGI(TAG, "Buffer de áudio cheio. Saindo do loop de preenchimento.");
                }
                break;
            }
        }

        // Envia o tamanho do buffer em hexadecimal
        char len_buf[20];
        int wlen = sprintf(len_buf, "%x\r\n", offset);
        if (esp_http_client_write(client, len_buf, wlen) <= 0) 
        {
            ESP_LOGI(TAG, "Falha na requisição HTTP POST: %d\n", wlen);
        }

        // Envia o conteúdo do buffer de áudio
        if (esp_http_client_write(client, (const char *)audio_buffer, offset) <= 0) 
        {
            ESP_LOGI(TAG, "HTTP POST request failed: %d\n", offset);
        }

        // Envia o conteúdo do buffer
        if (esp_http_client_write(client, (const char *)audio_buffer, offset) <= 0) 
        {
            ESP_LOGI(TAG, "HTTP POST request failed: %d\n", offset);
        }

        // Envia o terminador de chunk
        if (esp_http_client_write(client, "\r\n", 2) <= 0) 
        {
            ESP_LOGI(TAG, "HTTP POST request failed: %d\n", 2);
        }

        if(DEBUG)
            ESP_LOGI(TAG,"Fim da iteração %d", i);
    }

    // Enviar o chunk de término
    if (esp_http_client_write(client, "0\r\n\r\n", 5) <= 0) 
    {
        ESP_LOGI(TAG, "HTTP POST request failed: %d\n", 5);
    }

    if(DEBUG)
        ESP_LOGI(TAG,"Chunk de término enviado");

    /*
    * Faz a requisição POST;
    */
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else
    {
        ESP_LOGI(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

	/**
	* Encerra a conexão socket com o servidor e desaloca recursos;
    */
    esp_http_client_cleanup(client);

	return ESP_OK;
}


/**
 * @brief Task principal do cliente HTTP.
 * Realiza requisições POST de forma contínua em intervalos definidos.
 *
 * @param pvParameters Parâmetro de entrada para a task (não utilizado).
 */
static void http_client_task(void *pvParameters)
{
    for (;;)
    {
        // Realiza uma requisição POST
        http_post();

        // Aguarda 10 segundos antes de realizar a próxima requisição
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }

    // Deleta a task ao sair do loop principal
    vTaskDelete(NULL);
}

/**
 * @brief Inicia a task do cliente HTTP.
 * Cria uma task para realizar as requisições POST de forma periódica.
 */
void http_client_start(void)
{
    // Cria a task do cliente HTTP, associando-a ao core definido
    xTaskCreatePinnedToCore(&http_client_task, "http_client_task", HTTP_CLIENT_TASK_STACK_SIZE,
                            NULL,
                            HTTP_CLIENT_TASK_PRIORITY,
                            NULL,
                            HTTP_CLIENT_TASK_CORE_ID);
}