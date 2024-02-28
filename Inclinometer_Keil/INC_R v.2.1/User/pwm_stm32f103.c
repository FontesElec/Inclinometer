#include "stm32f10x.h"                  //Device header

void PWM_INIT(void)
{
    
    RCC->APB2ENR|=RCC_APB2ENR_IOPAEN|RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;                                           //включаем тактирование таймера
    
    GPIOA->CRL&=~GPIO_CRL_CNF2;                                                 //сбрасываем в 0 регистры конфига, иначе не работает	
    GPIOA->CRL|=GPIO_CRL_MODE2;                                                 //на выход 50мГц
    GPIOA->CRL|=GPIO_CRL_CNF2_1;                                                //порт А2 на альтернативную функцию
    TIM2->CCER|=TIM_CCER_CC3E;                                                  //3й канал, разрешаем управление пином
    
    TIM2->CCMR2|=TIM_CCMR2_OC3M_1|TIM_CCMR2_OC3M_2;                             //режим ШИМ прямой, управление внешним пином
    TIM2->ARR=65535;                                                            //значение перезагрузки
    TIM2->CCR3=0;                                                               //определяет коэффициент заполнения
    
    TIM2->CR1&=~(TIM_CR1_DIR|TIM_CR1_CMS);                                      //счёт вверх, выравнятьпо фронту
    TIM2->CR1|=TIM_CR1_CEN;                                                     //включить таймер
    
}

/*ДЛЯ УПРАВЛЕНИЯ ПОДСВЕТКОЙ С АЦП ВНЕСТИ СЛЕДУЩУЮ СТРОЧКУ
TIM2->CCR3=ADC1->JDR1;                                                   //шим управляется от данных ацп
*/