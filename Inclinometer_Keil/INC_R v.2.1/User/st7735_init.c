#include "stm32f10x.h"                  // Device header

void SS7735_INIT(void)
{
  SS7735_SPI_INIT();
//----------ЗАПИТКА ДИСПЛЕЯ. УДАЛИТЬ ПОСЛЕ СБОРКИ НА ПЛАТЕ!!!!!!!!!!!!!-----------
 
    RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
    GPIOA->CRL&=~(GPIO_CRL_CNF2);//сбрасываем регистры конфигурации порта 
    GPIOA->CRL|=GPIO_CRL_MODE2_0;//PA2 на выход под запитку дисплея;//выход на 2МГц
    GPIOA->BSRR|=GPIO_BSRR_BS2;//запитали подсветку дисплея
    
 //==========ЗАПИТКА ДИСПЛЕЯ. УДАЛИТЬ ПОСЛЕ СБОРКИ НА ПЛАТЕ!!!!!!!!!!!!!===========    

  GPIOA->CRL&=~(GPIO_CRL_CNF1|GPIO_CRL_CNF3);//сбрасываем регистры конфигурации порта 
  GPIOA->CRL|=(GPIO_CRL_MODE1_0|GPIO_CRL_MODE3_0);//выход на 2МГц
  
//==========================Предварительный сброс дисплея=======================  
    
  RES_HIGH;
  for(int i=36000;i>0;i--);
  RES_LOW;
  for(int i=36000;i>0;i--);
  RES_HIGH;

//------------------------------------------------------------------------------

//=============================ИНИЦИАЛИЗАЦИЯ ДИСПЛЕЯ============================
   CS_LOW;
   SS7735_SEND_COM(SWRESET);
   for(int i=36000;i>0;i--);
   SS7735_SEND_COM(SLPOUT);
   for(int i=36000;i>0;i--);
   SS7735_SEND_COM(COLMOD);//палитра
   SS7735_SEND_DAT(0b110);  //rgb 888
   
   SS7735_SEND_COM(MADCTL);//порядок отрисовки
   SS7735_SEND_DAT(NORM_ORIENT);
   
   SS7735_SEND_COM(FRMCTR1);//частота развёртки
   SS7735_SEND_DAT(0);
   SS7735_SEND_DAT(1);
   SS7735_SEND_DAT(1);
   
   
   SS7735_SEND_COM(DISPON);//включить дисплей
   CS_HIGH;
   
//------------------------------------------------------------------------------

   //for(int i=3600000;i>0;i--);
}
void SS7735_SPI_INIT(void)
{
    RCC->APB2ENR|=RCC_APB2ENR_IOPAEN| RCC_APB2ENR_AFIOEN;//тактирование порта А и запуск тактирования на альтернативную функцию
    GPIOA->CRL&= ~(GPIO_CRL_CNF4|GPIO_CRL_CNF5|GPIO_CRL_CNF7);//сбрасываем в 0 регистры конфига, иначе не работает	  
    GPIOA->CRL|=GPIO_CRL_MODE4|GPIO_CRL_MODE5|GPIO_CRL_MODE7;//50МГц на A4-A7
    GPIOA->CRL|=GPIO_CRL_CNF5_1|GPIO_CRL_CNF7_1;//порты А5,А7 на альтернативные функции, A4 на выход
    
    GPIOA->BSRR|=GPIO_BSRR_BS4;//установка 1 на SS(PA4)
        
    RCC->APB2ENR|=RCC_APB2ENR_SPI1EN;// тактирование на SPI1
    
    SPI1->CR1|=SPI_CR1_MSTR|SPI_CR1_SSI;// режим мастер, software slave management
    SPI1->CR1&=~(SPI_CR1_BR_2|SPI_CR1_BR_1|SPI_CR1_BR_0|SPI_CR1_CPOL|SPI_CR1_CPHA);// f/2 
    SPI1->CR2|=SPI_CR2_SSOE;
    SPI1->CR1|=SPI_CR1_SPE;//включить spi
}


void SS7735_SEND_COM(uint8_t data)
{
    DISP_COM;
    while((SPI1->SR & SPI_SR_TXE)==0);// while(~(SPI1->SR & SPI_SR_TXE)) не работает!
    SPI_DR_8bit = data;
    while(SPI1->SR & SPI_SR_BSY){(void)(SPI1->DR);}
}

void SS7735_SEND_DAT(uint8_t data)
{
    DISP_DAT;
    while((SPI1->SR & SPI_SR_TXE)==0);// while(~(SPI1->SR & SPI_SR_TXE)) не работает!
    SPI_DR_8bit = data;
    while(SPI1->SR & SPI_SR_BSY){(void)(SPI1->DR);}
}

void ORIENTATION(uint8_t param)
{
    CS_LOW;
    SS7735_SEND_COM(MADCTL);//порядок отрисовки
    SS7735_SEND_DAT(param);
    CS_HIGH;
}
