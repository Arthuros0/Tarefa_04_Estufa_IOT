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
#include "lib/webserver.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C

#define BUTTON_A 5
#define BUTTON_B 6
#define BUTTON_JOY 22


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

    ssd1306_draw_bitmap(&ssd,bitmap_estufa);

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
        vTaskDelay(pdMS_TO_TICKS(4000));
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
/*
void vWebServerTask(){

    UBaseType_t stack = uxTaskGetStackHighWaterMark(NULL);
    printf("Pilha restante (WebServer): %u palavras\n", stack);

    while (1)
    {
        
        cyw43_arch_poll(); // Necessário para manter o Wi-Fi ativo
        vTaskDelay(pdMS_TO_TICKS(100));      // Reduz o uso da CPU

        
    }
    

    //Desligar a arquitetura CYW43.
    cyw43_arch_deinit();

}
*/
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    // Essa função será chamada quando uma tarefa estourar a pilha
    printf("ESTOURO DE PILHA detectado na tarefa: %s\n", pcTaskName);
    while (1); // trava aqui para depuração
}



int main()
{
    stdio_init_all();

    init_led(LED_BLUE_PIN);
    init_led(LED_GREEN_PIN);
    init_led(LED_RED_PIN);
    init_led(LED_PIN);

    printf("\nSaiu do init wifi\n");

    init_button(BUTTON_A);
    init_button(BUTTON_B);
    init_button(BUTTON_JOY);

    gpio_set_irq_enabled_with_callback(BUTTON_A,GPIO_IRQ_EDGE_FALL,true,&button_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_B,GPIO_IRQ_EDGE_FALL,true,&button_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_JOY,GPIO_IRQ_EDGE_FALL,true,&button_callback);

    button_debounce=delayed_by_ms(get_absolute_time(), 200);
    debounce_serial=delayed_by_ms(get_absolute_time(), 500);

    if (init_wifi() != 0) {
        printf("Erro ao inicializar Wi-Fi\n");
        return -1;
    }

    struct tcp_pcb *server = tcp_new();
    tcp_bind(server, IP_ADDR_ANY, 80);
    server = tcp_listen(server);
    tcp_accept(server, tcp_server_accept);


    xTaskCreate(vDisplayTask, "Display Task",4096,NULL,tskIDLE_PRIORITY,NULL);

    xTaskCreate(vSimulaEstufaTask, "Estufa Task",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY,NULL);

    xTaskCreate(vMatrixTask, "Matrix Task",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY,NULL);

    xTaskCreate(vJoystickTask, "Joystick Task",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY,NULL);

    xTaskCreate(vWebServerTask, "WebServer", 4096, NULL, 2, NULL);
    
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
            menu_estufas=!menu_estufas;
            debounce_joy=delayed_by_ms(get_absolute_time(),200);
        }
        button_debounce=delayed_by_ms(get_absolute_time(), 250); //Atualiza debounce
    }

}