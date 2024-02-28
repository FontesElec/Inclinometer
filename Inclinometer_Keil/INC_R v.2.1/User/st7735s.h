#include "stm32f10x.h"                  // Device header


void SS7735_INIT(void);                                 //Инициализация работы дисплея  
void SS7735_SPI_INIT(void);                             //Иницализация шины SPI(1)
void SS7735_SEND_COM(uint8_t);                          //Отправка команд SPI(1)
void SS7735_SEND_DAT(uint8_t);                          //Отправка данных SPI(1)
void ORIENTATION(uint8_t);                              //Режим смены ориентации экрана


//------------------------------ДОПОЛНИТЕЛЬНО--------------------------------

#define RES_LOW         GPIOA->BSRR|=GPIO_BSRR_BR3;     //set 0 on RES(PA3)
#define RES_HIGH        GPIOA->BSRR|=GPIO_BSRR_BS3;     //set 1 on RES(PA3)
#define DISP_COM        GPIOA->BSRR|=GPIO_BSRR_BR1;     //set 0 on RS(PA1)
#define DISP_DAT        GPIOA->BSRR|=GPIO_BSRR_BS1;     //set 1 on RS(PA1)
#define CS_LOW          GPIOA->BSRR|=GPIO_BSRR_BR4;     //set 0 on SS(PA4)
#define CS_HIGH         GPIOA->BSRR|=GPIO_BSRR_BS4;     //set 1 on SS(PA4)
#define SPI_DR_8bit     (*(uint8_t*)&(SPI1->DR))        //send data to SPI in 8 bit mode
#define NORM_ORIENT     0b01000000                      //normal display orientation
#define INVERCE_ORIENT  0b10000000                      //reverse display orientation

//==============================ДОПОЛНИТЕЛЬНО=================================
