#include "webserver.h"

// === Função chamada quando uma nova conexão TCP é aceita ===
err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    printf("Nova conexão aceita\n");

    // Define a função de callback para tratar os dados recebidos
    tcp_recv(newpcb, tcp_server_recv);
    return ERR_OK;
}

// === Função chamada quando uma requisição é recebida ===
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    printf("Requisição recebida: %.*s\n", p->len, (char *)p->payload);

    // Resposta HTTP básica (sem malloc, sem HTML pesado)
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Estufa inteligente rodando com sucesso!\n";

    tcp_write(tpcb, response, strlen(response), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);

    pbuf_free(p);
    return ERR_OK;
}

// === Função chamada na tarefa do FreeRTOS apenas para manter a pilha funcionando ===

// === Inicializa Wi-Fi e conecta ===
int init_wifi() {
    uint8_t attempts = 0;
    const uint8_t max_attempts = 3;

    // Inicializa Wi-Fi
    while (cyw43_arch_init()) {
        printf("Falha ao inicializar Wi-Fi (tentativa %d)\n", attempts + 1);
        sleep_ms(1000);
        if (++attempts >= max_attempts) return -1;
    }

    cyw43_arch_gpio_put(LED_PIN, 0);
    cyw43_arch_enable_sta_mode();

    attempts = 0;
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
        printf("Falha ao conectar Wi-Fi (tentativa %d)\n", attempts + 1);
        sleep_ms(500);
        if (++attempts >= max_attempts) return -1;
    }

    printf("Conectado ao Wi-Fi\n");

    if (netif_default) {
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    } else {
        printf("netif_default está NULL (sem IP atribuído)\n");
    }

    return 0;
}
