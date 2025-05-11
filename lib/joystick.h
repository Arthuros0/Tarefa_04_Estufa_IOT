#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "estufa.h"

#define Y_JOY 26  
#define X_JOY 27  
#define Y_CHANNEL 0 //Canal do ADC vinculado ao Y_JOY
#define X_CHANNEL 1 //Canal do ADC vinculado ao X_JOY

#define MAX_JOY 3700
#define MIN_JOY 400

extern bool menu_estufas;
extern bool sub_menu_estufas;
extern bool menu_status;
extern uint8_t indice_menu;
extern absolute_time_t debounce_joy;


int64_t joy_callback(alarm_id_t id, void *user_data);
void joystick_setup();