#include "stm32f10x.h"                  //Device header
void ADC_INIT(void)
{
   RCC->APB2ENR|=RCC_APB2ENR_ADC1EN;
   ADC1->CR2|=ADC_CR2_CAL;//калибровка АЦП
   while(!(ADC1->CR2 & ADC_CR2_CAL));//ждём конца калибровки
   ADC1->SMPR2|=(ADC_SMPR2_SMP8_0|ADC_SMPR2_SMP8_1|ADC_SMPR2_SMP8_2);//239.5 циклов
   ADC1->CR2|=ADC_CR2_JEXTSEL;//старт преобразования битом старт
   ADC1->CR2 |= ADC_CR2_JEXTTRIG|ADC_CR2_CONT; //Разрешаем внешний запуск инжектированной группы, режим постоянного измерения
   ADC1->CR1 |= ADC_CR1_JAUTO; //Разрешить преобразование инжектированной группы после регулярной. Не понятно зачем, но без этого не работает
   ADC1->JSQR |= (8 <<15);//запускаем 8й канал под измерения
   ADC1->CR2 |= ADC_CR2_ADON;//Теперь включаем АЦП
   ADC1->CR2 |= ADC_CR2_JSWSTART; //Запуск преобразований
   while (!(ADC1->SR & ADC_SR_JEOC)); //ждем пока первое преобразование завершится
}    

