#ifndef MATRIX_LEDS_H
#define MATRIX_LEDS_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_matrix.pio.h"
#include "estufa.h"

extern uint sm;

void init_led(uint8_t pin);

void setup_led_matrix();

void muda_estado(uint8_t pin);

uint8_t obter_index(uint8_t i);

void desenha_frame(const uint32_t matriz[10][25],uint8_t frame);

void altera_status();

void desenha_status();

extern uint32_t status[25];

#endif