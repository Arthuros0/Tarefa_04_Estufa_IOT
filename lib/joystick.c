#include "joystick.h"
#include <stdio.h>

bool menu_estufas=false;
bool sub_menu_estufas=false;
bool menu_status=false;

uint8_t indice_menu=0;

absolute_time_t debounce_joy;


void joystick_setup(){
  adc_init();
  adc_gpio_init(Y_JOY);
  adc_gpio_init(X_JOY);
}

int64_t joy_callback(alarm_id_t id, void *user_data){
  if (menu_estufas)
  {
    if(time_reached(debounce_joy)){
      adc_select_input(Y_CHANNEL);
      if(!sub_menu_estufas && !menu_status){
        if ((adc_read()) > MAX_JOY)
        {
          indice_menu--;
          if(indice_menu>2)indice_menu=2;
          debounce_joy=delayed_by_ms(get_absolute_time(),250);
          return 200000;
        }else if ((adc_read()) < MIN_JOY)
        {
          indice_menu++;
          if(indice_menu>2)indice_menu=0;
          debounce_joy=delayed_by_ms(get_absolute_time(),250);
          return 200000;
        }
      }
      adc_select_input(X_CHANNEL);

      if ((adc_read()) > MAX_JOY)
      {
        if (menu_status)
        {
          menu_status=false;
        }else{
          sub_menu_estufas=true;
        }
        debounce_joy=delayed_by_ms(get_absolute_time(),250);
        return 200000;
      }else if ((adc_read()) < MIN_JOY)
      {
        if (sub_menu_estufas)
        {
          sub_menu_estufas=false;
        }else{
          menu_status=true;
        }
        debounce_joy=delayed_by_ms(get_absolute_time(),250);
        return 200000;
      }

    }
    return 250000;
  
  }
  return 0;
}

