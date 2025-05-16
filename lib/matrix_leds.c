
#include "matrix_leds.h"


#define NUM_PIXELS 25

#define ASPERSOR 0x03041400//Azul
#define IRRIGACAO 0x19000600 //verde
#define VENTILACAO 0x19001900 //ciano
#define FERTILIZACAO 0x19190000 //amarelo
#define DESLIGADO 0x00000000

#define OUT_PIN 7

PIO pio=pio0;
uint sm=0;

void init_led(uint8_t pin){
    gpio_init(pin);
    gpio_set_dir(pin,GPIO_OUT);
}

void muda_estado(uint8_t pin){
    gpio_put(pin,!gpio_get(pin));
}

uint8_t obter_index(uint8_t i) {
    uint8_t x = i % 5;  // Coluna
    uint8_t y = i / 5;  // Linha
    return (y % 2 == 0) ? (y * 5 + x) : (y * 5 + (4 - x));
}

void desenha_frame(const uint32_t matriz[10][25],uint8_t frame){

    for (uint8_t i = 0; i < NUM_PIXELS; i++)
    {
        uint8_t pos=obter_index(i);         
        pio_sm_put_blocking(pio0,sm,matriz[frame][pos]);
    }
    
}

void altera_status(){
    for (uint8_t i = 0; i < NUM_ESTUFAS; i++)
    {   
        uint8_t coluna=i*2;
        status[coluna] = estufas[i].status_umidade ? ASPERSOR : DESLIGADO;
        status[5 + coluna] = estufas[i].status_irrigacao ? IRRIGACAO : DESLIGADO;
        status[10 + coluna] = estufas[i].status_ventilacao ? VENTILACAO : DESLIGADO;
        status[15 + coluna] = estufas[i].status_fertilizante ? FERTILIZACAO : DESLIGADO;
        
    }
    
}

void desenha_status(){
    if (status_alterado)
    {
        altera_status();
        for (uint8_t i = 0; i < NUM_PIXELS; i++)
        {
            uint8_t pos=obter_index(i);         
            pio_sm_put_blocking(pio0,sm,status[24-pos]);
        }
        status_alterado=false;
        //sleep_us(100);
    }
}


void setup_led_matrix() {
    bool ok;
    // Configura o clock para 133 MHz
    ok = set_sys_clock_khz(133000, false);
    //stdio_init_all();

    printf("Iniciando a transmissão PIO\n");
    if (ok) printf("Clock configurado para %ld Hz\n", clock_get_hz(clk_sys));

    // Configuração do PIO
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);
}


uint32_t status[25] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
    };
