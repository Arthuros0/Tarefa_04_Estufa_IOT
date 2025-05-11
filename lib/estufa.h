#ifndef ESTUFA_H
#define ESTUFA_H

#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>

#define NUM_ESTUFAS 3
#define MIN_UMIDADE_AR 55
#define MAX_UMIDADE_AR 70
#define MIN_UMIDADE_SOLO 40
#define MAX_UMIDADE_SOLO 55
#define MIN_TEMPERATURA 20
#define MAX_TEMPERATURA 30

#define MIN_VALOR 0
#define MAX_VALOR 100

#define TAXA_UMIDADE_SOLO 1.3
#define TAXA_DEC_UMIDADE_SOLO 0.5
#define TAXA_UMIDADE_AR 1.8
#define TAXA_DEC_UMIDADE_AR 0.5
#define TAXA_TEMPERATURA 0.7
#define TAXA_DEC_TEMPERATURA 0.3

extern bool status_alterado;

extern struct repeating_timer timer_clima;

extern absolute_time_t debounce_serial;


typedef struct estufa
{
  uint8_t id;
  bool status_umidade;
  bool status_irrigacao;
  bool status_ventilacao;
  bool status_fertilizante;
  float umidade_ar;
  float umidade_solo;
  float temperatura;
}estufa;

extern estufa estufas[3];

void atualiza_estado_estufa();
void imprime_informacoes();
bool simula_clima(struct repeating_timer *t);
void alterna_fertirrigacao(uint8_t i);
void init_estufas();

#endif