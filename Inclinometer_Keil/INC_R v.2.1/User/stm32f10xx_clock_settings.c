/*
    Файл настройки источника и частоты тактирования под ядро Cortex M3
    В текущей конфигурации происходит переключение на внутренний генератор HSI и перестройка на PLL шину

*/


#include "stm32f10x.h"                  //Device header

void CLK_INIT(void)
{
    RCC->CFGR|=RCC_CFGR_PLLMULL12;                      //множитель частоты x12 (96МГц)
    RCC->CFGR&=~RCC_CFGR_PLLSRC;                        //в качестве источника HSI с делителем частоты на 2(48МГц)
    RCC->CR|=RCC_CR_PLLON;                              //запуск шины PLL
    while(RCC->CR & RCC_CR_PLLRDY);                     //ждём сигнала готовности
    RCC->CFGR&=~(RCC_CFGR_SW_1|RCC_CFGR_SW_0);          //сбрасываем настройки источника (HSI теперь источник такта)
    RCC->CFGR|=RCC_CFGR_SW_1;                           //тактируем от линии PLL
    
}

