#include "stm32f10x.h"                  // Device header

void ADXL345_INIT(void);
void ADXL345_SPI_INIT(void);
void ADXL345_SPI_SEND(uint8_t,uint8_t);
int16_t ADXL345_SPI_RECEIVE_16BIT(uint8_t);


void ADXL345_INIT(void)
{
    ADXL345_SPI_INIT();
    ADXL345_SPI_SEND(DATA_FORMAT,ADXL345_D_FORMAT);
    ADXL345_SPI_SEND(BW_RATE,ADXL345_MAX_D_RATE);
    ADXL345_SPI_SEND(POWER_CTL,ADXL345_MEASURE);
    
    
}

void ADXL345_SPI_INIT(void)
{
    RCC->APB2ENR|=RCC_APB2ENR_IOPBEN| RCC_APB2ENR_AFIOEN;//тактирование порта B и запуск тактирования на альтернативную функцию
    GPIOB->CRH&= ~(GPIO_CRH_CNF12|GPIO_CRH_CNF13|GPIO_CRH_CNF14|GPIO_CRH_CNF15);//сбрасываем в 0 регистры конфига, иначе не работает	  
    GPIOB->CRH|=GPIO_CRH_MODE12|GPIO_CRH_MODE13|GPIO_CRH_MODE14|GPIO_CRH_MODE15;//10МГц на B12-B15
    GPIOB->CRH|=GPIO_CRH_CNF13_1|GPIO_CRH_CNF14_1|GPIO_CRH_CNF15_1;//порты B13-B15 на альтернативные функции, B12 на выход
    
    GPIOB->BSRR|=GPIO_BSRR_BS12;//установка 1 на SS(PB12)
        
    RCC->APB1ENR|=RCC_APB1ENR_SPI2EN;//тактирование на SPI2
    
    
    SPI2->CR1|=SPI_CR1_MSTR|SPI_CR1_SSI|SPI_CR1_CPOL|SPI_CR1_CPHA;//включить SPI2, режим мастер, f/4 , software slave management
    SPI2->CR2|=SPI_CR2_SSOE;
    SPI2->CR1|=SPI_CR1_SPE;//включить spi
    for(int i=200000;i>0;i--);       
    
}

void ADXL345_SPI_SEND(uint8_t adr, uint8_t data)
{
    
    ADXL345_CS_LOW;
    ADXL345_SPI_DR_8bit=adr;
    while((SPI2->SR & SPI_SR_RXNE)==0);
    (void)(SPI2->DR);
    ADXL345_SPI_DR_8bit = data;
    while((SPI2->SR & SPI_SR_RXNE)==0);
    (void)(SPI2->DR);
    while(SPI2->SR & SPI_SR_BSY){(void)(SPI2->DR);}
    ADXL345_CS_HIGH;
    for (int i=0; i<50; i++);//ждём включения SPI слэйва (иначе отправит нули на первые запросы)
    //SPI2->CR1|=SPI_CR1_SPE;//включить spi
    
}

int16_t ADXL345_SPI_RECEIVE_16BIT(uint8_t adr)
{
    int16_t data=0;
    ADXL345_CS_LOW;
    while((SPI2->SR & SPI_SR_TXE)==0);//ждём, когда освободится буфер отправки
    ADXL345_SPI_DR_8bit=adr|ADXL345_SPI_READ;//запрос первого байта
    while((SPI2->SR & SPI_SR_RXNE)==0);
    (void)(SPI2->DR);//пропускаем первое сообщение
    ADXL345_SPI_DR_8bit=0;
    while((SPI2->SR & SPI_SR_RXNE)==0);
    data=(uint8_t)(SPI2->DR);//старший байт параметра
    ADXL345_SPI_DR_8bit=0;
    while((SPI2->SR & SPI_SR_RXNE)==0);
    data=data|(((uint8_t)(SPI2->DR))<<8);//и младший байт параметра
    while((SPI2->SR & SPI_SR_BSY)!=0);
    ADXL345_CS_HIGH;
    
    
    return data; 
}

