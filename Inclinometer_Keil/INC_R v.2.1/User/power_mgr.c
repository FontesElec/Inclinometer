#include "stm32f10x.h"                  //Device header

void BATT_CHECK(void);                  //Проверка статуса батареи (отобразить заряд, низкий заряд, зарядка)
void POWER_LOW(void);                   //Действия при низком заряде
void POWER_OFF(void);                   //Отключаем питание батареи



void BATT_CHECK(void)
{
    //Настройка АЦП: верхний диапазон 3600, нижний - 3150, рабочая область 450 единиц    
    uint16_t u_bat=ADC1->JDR1;
    //u_bat=2900;                                           //проверка на срабатывание автоотключения
    if(u_bat<=3150)
    {
        if(!((GPIOB->IDR)&GPIO_IDR_IDR8))                   //если низкое напряжение и не нажата кнопка вкл
        {
            POWER_LOW();
        }
                 
    }
    else
    {
        DRAW_BATTERY_WIDGET(128,4,current.statusbar,current.background,u_bat);                     //нарисовали батарейку
    }

}



void POWER_LOW(void)
{
   DISP_REFRESH();
   DRAW_IMAGE(38,60,low_charge_sign,current.background,current.statusbar);                  //предупреждение о низком уровне заряда
   for(int j=9000000;j>0;j--);
   GPIOA->BSRR|=GPIO_BSRR_BR2;                      //отключаем подсветку дисплея
   for(int j=700000;j>0;j--);
   GPIOB->BSRR|=GPIO_BSRR_BR9;                      //отключаем поддержку сигнала на LDO (выключение)
}

void POWER_OFF(void)
{
   for(int i=70000;i>0;i--); 
   if(((GPIOB->IDR)&GPIO_IDR_IDR8))                     //если ещё нажата кнопка POWER
   {
       GPIOA->BSRR|=GPIO_BSRR_BR2;                      //отключаем подсветку дисплея
      
       for(int j=7000000;j>0;j--);
                      
       GPIOB->BSRR|=GPIO_BSRR_BR9;                      //отключаем поддержку сигнала на LDO (выключение)
   }
}
