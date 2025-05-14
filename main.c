#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "lib/ssd1306.h"
#include "lib/estufa.h"
#include "lib/joystick.h"
#include "lib/matrix_leds.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
//#include "lib/webserver.h"

#include <stdio.h>               // Biblioteca padrão para entrada e saída
#include <string.h>              // Biblioteca manipular strings
#include <stdlib.h>              // funções para realizar várias operações, incluindo alocação de memória dinâmica (malloc)

#include "pico/stdlib.h"         // Biblioteca da Raspberry Pi Pico para funções padrão (GPIO, temporização, etc.)
#include "hardware/adc.h"        // Biblioteca da Raspberry Pi Pico para manipulação do conversor ADC
#include "pico/cyw43_arch.h"     // Biblioteca para arquitetura Wi-Fi da Pico com CYW43  

#include "lwip/pbuf.h"           // Lightweight IP stack - manipulação de buffers de pacotes de rede
#include "lwip/tcp.h"            // Lightweight IP stack - fornece funções e estruturas para trabalhar com o protocolo TCP
#include "lwip/netif.h"          // Lightweight IP stack - fornece funções e estruturas para trabalhar com interfaces de rede (netif)

// Credenciais WIFI - Tome cuidado se publicar no github!
#define WIFI_SSID "EmbarcaTech"
#define WIFI_PASSWORD "Restic_37"

// Definição dos pinos dos LEDs
#define LED_PIN CYW43_WL_GPIO_LED_PIN   // GPIO do CI CYW43
#define LED_BLUE_PIN 12                 // GPIO12 - LED azul
#define LED_GREEN_PIN 11                // GPIO11 - LED verde
#define LED_RED_PIN 13                  // GPIO13 - LED vermelho

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C

#define BUTTON_A 5
#define BUTTON_B 6
#define BUTTON_JOY 22

// Inicializar os Pinos GPIO para acionamento dos LEDs da BitDogLab
void gpio_led_bitdog(void);

// Função de callback ao aceitar conexões TCP
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);

// Função de callback para processar requisições HTTP
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

// Leitura da temperatura interna
float temp_read(void);

// Tratamento do request do usuário
void user_request(char **request);


ssd1306_t ssd; //Váriavel do tipo ssd1306_t, armazena as informações relacionados ao display

absolute_time_t button_debounce;

void button_callback(uint gpio,uint32_t events);
void init_button(uint8_t pin);

void vMatrixTask(){

    setup_led_matrix();

    while (1)
    {
        desenha_status();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


void vDisplayTask(){

    i2c_init(I2C_PORT,400*5000);
    init_i2c_pins(I2C_SDA,I2C_SCL);
    init_display(&ssd,ENDERECO,I2C_PORT);

    //ssd1306_draw_bitmap(&ssd,bitmap_estufa);

    while (1)
    {
        cor=!cor;
        if(!menu_estufas)ssd1306_draw_bitmap(&ssd,bitmap_estufa);
        desenha_menu(&ssd);
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

void vSimulaEstufaTask(){
    init_estufas();

    while (1)
    {
        simula_clima();
        vTaskDelay(pdMS_TO_TICKS(5000));
    }   
}

void vJoystickTask(){

    joystick_setup();

    while (1)
    {
        joy_read();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    
}

void vWebServerTask(){

    //UBaseType_t stack = uxTaskGetStackHighWaterMark(NULL);
    //printf("Pilha restante (WebServer): %u palavras\n", stack);

    while (1)
    {
        cyw43_arch_poll(); // Necessário para manter o Wi-Fi ativo     
        vTaskDelay(pdMS_TO_TICKS(100));   
    }
    //Desligar a arquitetura CYW43.
    cyw43_arch_deinit();
}

/*
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    // Essa função será chamada quando uma tarefa estourar a pilha
    printf("ESTOURO DE PILHA detectado na tarefa: %s\n", pcTaskName);
    while (1); // trava aqui para depuração
}
*/

int main()
{
    stdio_init_all();

    init_led(LED_BLUE_PIN);
    init_led(LED_GREEN_PIN);
    init_led(LED_RED_PIN);
    init_led(LED_PIN);

    init_button(BUTTON_A);
    init_button(BUTTON_B);
    init_button(BUTTON_JOY);

    gpio_set_irq_enabled_with_callback(BUTTON_A,GPIO_IRQ_EDGE_FALL,true,&button_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_B,GPIO_IRQ_EDGE_FALL,true,&button_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_JOY,GPIO_IRQ_EDGE_FALL,true,&button_callback);

    button_debounce=delayed_by_ms(get_absolute_time(), 200);
    debounce_serial=delayed_by_ms(get_absolute_time(), 500);

    sleep_ms(5000);

    // Inicializar os Pinos GPIO para acionamento dos LEDs da BitDogLab
    gpio_led_bitdog();
    printf("Inicializando arquitetura");
    //Inicializa a arquitetura do cyw43
    while (cyw43_arch_init())
    {
        printf("Falha ao inicializar Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }
    printf("Inicializou\n");
    // GPIO do CI CYW43 em nível baixo
    cyw43_arch_gpio_put(LED_PIN, 0);

    // Ativa o Wi-Fi no modo Station, de modo a que possam ser feitas ligações a outros pontos de acesso Wi-Fi.
    cyw43_arch_enable_sta_mode();
    printf("Modo Station ativado\n");

    // Conectar à rede WiFI - fazer um loop até que esteja conectado
    printf("Conectando ao Wi-Fi...\n");
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 180000))
    {
        printf("Falha ao conectar ao Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }
    printf("Conectado ao Wi-Fi\n");

    // Caso seja a interface de rede padrão - imprimir o IP do dispositivo.
    if (netif_default)
    {
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }

    // Configura o servidor TCP - cria novos PCBs TCP. É o primeiro passo para estabelecer uma conexão TCP.
    struct tcp_pcb *server = tcp_new();
    if (!server)
    {
        printf("Falha ao criar servidor TCP\n");
        return -1;
    }

    //vincula um PCB (Protocol Control Block) TCP a um endereço IP e porta específicos.
    if (tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK)
    {
        printf("Falha ao associar servidor TCP à porta 80\n");
        return -1;
    }

    // Coloca um PCB (Protocol Control Block) TCP em modo de escuta, permitindo que ele aceite conexões de entrada.
    server = tcp_listen(server);

    // Define uma função de callback para aceitar conexões TCP de entrada. É um passo importante na configuração de servidores TCP.
    tcp_accept(server, tcp_server_accept);
    printf("Servidor ouvindo na porta 80\n");

    xTaskCreate(vDisplayTask, "Display Task",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY,NULL);

    xTaskCreate(vSimulaEstufaTask, "Estufa Task",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY,NULL);

    //xTaskCreate(vMatrixTask, "Matrix Task",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY,NULL);

    xTaskCreate(vJoystickTask, "Joystick Task",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY,NULL);

    xTaskCreate(vWebServerTask, "WebServer",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY,NULL);
    
    vTaskStartScheduler(); //Inicia o agendador do FreeRTOS
    panic_unsupported();
}


void init_button(uint8_t pin){
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
}
  
void button_callback(uint gpio,uint32_t events){
    if (time_reached(button_debounce)) //Verifica se o tempo de debounce foi atigindo
    {
        if (gpio == BUTTON_A)
        {
            printf("Botao A\n");
            if (sub_menu_estufas || menu_status)
            {
                printf("Fertilizante\n");
                alterna_fertirrigacao(indice_menu);
            }
            
        }else if(gpio == BUTTON_B){
            reset_usb_boot(0, 0);
        }else if(gpio == BUTTON_JOY){
            printf("Botao Joystick\n");
            menu_estufas=!menu_estufas;
            debounce_joy=delayed_by_ms(get_absolute_time(),200);
        }
        button_debounce=delayed_by_ms(get_absolute_time(), 250); //Atualiza debounce
    }

}

// Inicializar os Pinos GPIO para acionamento dos LEDs da BitDogLab
void gpio_led_bitdog(void){
    // Configuração dos LEDs como saída
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_put(LED_BLUE_PIN, false);
    
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_put(LED_GREEN_PIN, false);
    
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_put(LED_RED_PIN, false);
}

// Função de callback ao aceitar conexões TCP
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    tcp_recv(newpcb, tcp_server_recv);
    return ERR_OK;
}

// Tratamento do request do usuário - digite aqui
void user_request(char **request){

    
};


// Função de callback para processar requisições HTTP
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (!p)
    {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    // Alocação do request na memória dinámica
    char *request = (char *)malloc(p->len + 1);
    memcpy(request, p->payload, p->len);
    request[p->len] = '\0';

    printf("Request: %s\n", request);

    // Tratamento de request - Controle dos LEDs
    user_request(&request);
    
    // Leitura da temperatura interna
    float temperature = temp_read();

    // Cria a resposta HTML
    char html[2048];

    // Instruções html do webserver
    snprintf(html, sizeof(html), // Formatar uma string e armazená-la em um buffer de caracteres
            "<html><body><table border=1><tr><th>Estufa</th><th>Temperatura</th><th>Umidade Ar</th><th>Umidade Solo</th><th>Aspersor</th><th>Irrigacao</th><th>Ventilacao</th><th>Fertirrigacao</th></tr>"
            "<tr><td>1</td><td>%.1f</td><td>%.1f</td><td>%.1f</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>"
            "<tr><td>2</td><td>%.1f</td><td>%.1f</td><td>%.1f</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>"
            "<tr><td>3</td><td>%.1f</td><td>%.1f</td><td>%.1f</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>"
            "</table></body></html>",
            estufas[0].temperatura, estufas[0].umidade_ar, estufas[0].umidade_solo,
            estufas[0].status_umidade ? "On" : "Off", estufas[0].status_irrigacao ? "On" : "Off",
            estufas[0].status_ventilacao ? "On" : "Off", estufas[0].status_fertilizante ? "On" : "Off",

            estufas[1].temperatura, estufas[1].umidade_ar, estufas[1].umidade_solo,
            estufas[1].status_umidade ? "On" : "Off", estufas[1].status_irrigacao ? "On" : "Off",
            estufas[1].status_ventilacao ? "On" : "Off", estufas[1].status_fertilizante ? "On" : "Off",

            estufas[2].temperatura, estufas[2].umidade_ar, estufas[2].umidade_solo,
            estufas[2].status_umidade ? "On" : "Off", estufas[2].status_irrigacao ? "On" : "Off",
            estufas[2].status_ventilacao ? "On" : "Off", estufas[2].status_fertilizante ? "On" : "Off"
            );

    // Escreve dados para envio (mas não os envia imediatamente).
    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);

    // Envia a mensagem
    tcp_output(tpcb);

    //libera memória alocada dinamicamente
    free(request);
    
    //libera um buffer de pacote (pbuf) que foi alocado anteriormente
    pbuf_free(p);

    return ERR_OK;
}
