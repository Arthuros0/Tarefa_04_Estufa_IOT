#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"

#include "FreeRTOS.h"
#include "task.h"

#define WIFI_SSID     "EmbarcaTech"
#define WIFI_PASSWORD "Restic_37"

#define LED_PIN          CYW43_WL_GPIO_LED_PIN
#define LED_BLUE_PIN     12
#define LED_GREEN_PIN    11
#define LED_RED_PIN      13

// Inicializa conexão Wi-Fi
int init_wifi(void);

// Função da tarefa FreeRTOS que mantém a pilha de rede
void vWebServerTask(void *params);

// Funções do servidor TCP
err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
