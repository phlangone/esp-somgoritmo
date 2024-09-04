/******************************************************************************
 * @file    tasks_common.h
 * @brief   Definições comuns de tasks para o projeto.
 * 
 * Este arquivo define as configurações de stack, prioridade e core para
 * diferentes tasks no projeto, incluindo WiFi, cliente HTTP, monitor de cliente
 * HTTP e a tarefa para o microfone MEMS INMP441.
 * 
 * @version 1.0
 * @date    2024-09-03
 * 
 * @note    Ajuste os tamanhos de stack e prioridades conforme necessário para o
 *          seu projeto específico e as necessidades de gerenciamento de tarefas.
 * 
 * @copyright Copyright (c) 2024
 *           Todos os direitos reservados.
 ******************************************************************************
 */

#ifndef MAIN_TASKS_COMMON_H_
#define MAIN_TASKS_COMMON_H_

// Task para a aplicação WiFi
#define WIFI_APP_TASK_STACK_SIZE        4096       ///< Tamanho da stack para a task de aplicação WiFi
#define WIFI_APP_TASK_PRIORITY          5          ///< Prioridade da task de aplicação WiFi
#define WIFI_APP_TASK_CORE_ID           0          ///< Core onde a task de aplicação WiFi será executada

// Task para o cliente HTTP
#define HTTP_CLIENT_TASK_STACK_SIZE     10*1024    ///< Tamanho da stack para a task do cliente HTTP
#define HTTP_CLIENT_TASK_PRIORITY       5          ///< Prioridade da task do cliente HTTP
#define HTTP_CLIENT_TASK_CORE_ID        0          ///< Core onde a task do cliente HTTP será executada

// Task para monitorar o servidor HTTP
#define HTTP_CLIENT_MONITOR_STACK_SIZE  4096       ///< Tamanho da stack para a task de monitoramento do cliente HTTP
#define HTTP_CLIENT_MONITOR_PRIORITY    3          ///< Prioridade da task de monitoramento do cliente HTTP
#define HTTP_CLIENT_MONITOR_CORE_ID     0          ///< Core onde a task de monitoramento do cliente HTTP será executada

// Task para o microfone MEMS INMP441
#define INMP441_TASK_STACK_SIZE         4096       ///< Tamanho da stack para a task do microfone INMP441
#define INMP441_TASK_PRIORITY           5          ///< Prioridade da task do microfone INMP441
#define INMP441_TASK_CORE_ID            1          ///< Core onde a task do microfone INMP441 será executada

#endif /* MAIN_TASKS_COMMON_H_ */
