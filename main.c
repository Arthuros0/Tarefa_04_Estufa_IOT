#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "lib/ssd1306.h"
#include "lib/estufa.h"
#include "lib/joystick.h"
#include "lib/matrix_leds.h"

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



int main()
{
    stdio_init_all();

    i2c_init(I2C_PORT,400*5000);
    init_i2c_pins(I2C_SDA,I2C_SCL);

    setup_led_matrix();

    init_button(BUTTON_A);
    init_button(BUTTON_B);
    init_button(BUTTON_JOY);

    joystick_setup();

    gpio_set_irq_enabled_with_callback(BUTTON_A,GPIO_IRQ_EDGE_FALL,true,&button_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_B,GPIO_IRQ_EDGE_FALL,true,&button_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_JOY,GPIO_IRQ_EDGE_FALL,true,&button_callback);

    init_display(&ssd,ENDERECO,I2C_PORT);

    button_debounce=delayed_by_ms(get_absolute_time(), 200);
    debounce_serial=delayed_by_ms(get_absolute_time(), 500);

    init_estufas();

    ssd1306_draw_bitmap(&ssd,bitmap_estufa);
    while (!menu_estufas)
    {
        sleep_ms(200);
    }

    add_repeating_timer_ms(4000,simula_clima,NULL,&timer_clima);

    while (true) {
        cor=!cor;
        if(!menu_estufas)ssd1306_draw_bitmap(&ssd,bitmap_estufa);
        desenha_menu(&ssd);
        desenha_status();
        sleep_ms(500);
    }
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
            add_alarm_in_ms(100,joy_callback,NULL, false);
            debounce_joy=delayed_by_ms(get_absolute_time(),250);
        }
        button_debounce=delayed_by_ms(get_absolute_time(), 250); //Atualiza debounce
    }

}