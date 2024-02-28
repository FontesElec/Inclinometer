void KEY_INIT(void);                    //настройка работы кнопок
void EXTI_CONFIG(void);                 //настройка внешних прерываний




void KEY_INIT(void)
{
   RCC->APB2ENR|=RCC_APB2ENR_IOPBEN|RCC_APB2ENR_AFIOEN;
   
    //Подготовка узла питания (настройка линии удержания питания)

    RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
    GPIOB->CRH&=~GPIO_CRH_CNF9;
    GPIOB->CRH|=GPIO_CRH_MODE9;
    GPIOB->BSRR|=GPIO_BSRR_BS9;

    //---------------------------------

    
   GPIOB->CRL&=~(GPIO_CRL_MODE5|GPIO_CRL_MODE6|GPIO_CRL_MODE7);     //сбрасываем выводы под кнопки
   GPIOB->CRH&=~(GPIO_CRH_MODE8);                                   //сбрасываем выводы под кнопки 
    
   GPIOB->CRL&=~(GPIO_CRL_CNF5|GPIO_CRL_CNF6|GPIO_CRL_CNF7);        //сброс параметров конфига
   GPIOB->CRH&=~(GPIO_CRH_CNF8);                                    //сброс параметров конфига 
    
   GPIOB->CRL|=(GPIO_CRL_CNF5_1|GPIO_CRL_CNF6_1|GPIO_CRL_CNF7_1);   //настройка на вход без подтяжки
   GPIOB->CRH|=(GPIO_CRH_CNF8_1);                                   //настройка на вход без подтяжки

   //while(((GPIOB->IDR)&GPIO_IDR_IDR8)){};                         //ждём когда отпустят кнопку питания
}

void EXTI_CONFIG(void)
{
    //выбор порта и пина для подключения внешнего прерывания
    AFIO->EXTICR[1]|=AFIO_EXTICR2_EXTI5_PB|AFIO_EXTICR2_EXTI6_PB|AFIO_EXTICR2_EXTI7_PB;
    AFIO->EXTICR[2]|=AFIO_EXTICR3_EXTI8_PB;
    
    //реакция на нарастающий фронт
    EXTI->RTSR|=EXTI_RTSR_TR5|EXTI_RTSR_TR6|EXTI_RTSR_TR7|EXTI_RTSR_TR8;
   
    //Установка маски прерываний
    EXTI->IMR|=EXTI_IMR_MR5|EXTI_IMR_MR6|EXTI_IMR_MR7|EXTI_IMR_MR8;
    
    //Разрешаем прерывания по EXTI
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}


