#include "stm32f10x.h"                  //Device header

void CHANGE_THEME(uint8_t n_current_theme)
{
   
   struct theme first = {
        .background = 0xD0AA60, 
        .statusbar  = 0xA90400, 
        .big_digits = 0x5E0000, 
        .ball       = 0xFF1F00
   };
    
   switch (n_current_theme){
       case 1: current=first;
       default: current=first;
   }
}

