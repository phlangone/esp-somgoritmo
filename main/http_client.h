/******************************************************************************
 * @file    http_client.h
 * @brief   Cabeçalho para o cliente HTTP do projeto.
 * 
 * Este arquivo define as funções e estruturas relacionadas ao cliente HTTP
 * utilizado no projeto. Inclui a URL do servidor HTTP, as mensagens
 * para monitoramento HTTP e funções para enviar mensagens para a fila e iniciar
 * o cliente HTTP.
 * 
 * @version 1.0
 * @date    2024-09-03
 * 
 * @note    Certifique-se de que a URL do servidor está correta e corresponde ao
 *          servidor de destino. Adapte as mensagens e estruturas conforme necessário
 *          para a aplicação específica.
 * 
 * @copyright Copyright (c) 2024
 *           Todos os direitos reservados.
 ******************************************************************************
 */

#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include "freertos/FreeRTOS.h"

// Dados do servidor
#define HTTP_SERVER_URL "http://192.168.82.230:8000/upload"

/**
 * @brief Mensagens para o monitor HTTP
 */
typedef enum http_server_message
{
    HTTP_MSG_WIFI_CONNECT_INIT = 0,    ///< Inicialização da conexão Wi-Fi
    HTTP_MSG_WIFI_CONNECT_SUCCESS,     ///< Conexão Wi-Fi bem-sucedida
    HTTP_MSG_WIFI_CONNECT_FAIL,        ///< Falha na conexão Wi-Fi
} http_server_message_e;

/**
 * @brief Estrutura para a mensagem da fila
 */
typedef struct http_server_queue_message
{
    http_server_message_e msgID; ///< Identificador da mensagem
} http_server_queue_message_t;

/**
 * @brief Envia uma mensagem para a fila
 * 
 * @param msgID Identificador da mensagem do tipo http_server_message_e.
 * @return pdTRUE se o item foi enviado com sucesso para a fila, caso contrário pdFALSE.
 * @note Expanda a lista de parâmetros conforme a aplicação
 */
BaseType_t http_server_monitor_send_message(http_server_message_e msgID);

/**
 * @brief Inicia o cliente HTTP
 */
void http_client_start(void);

#endif /* HTTP_CLIENT_H_*/