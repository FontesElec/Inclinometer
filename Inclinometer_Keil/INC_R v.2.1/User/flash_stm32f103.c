#include "stm32f10x.h"                  //Device header

#define             LEAST_PAGE_ADDR             0x800FC00
#define             RDPRT_KEY                   0x00A5
#define             KEY1                        0x45670123
#define             KEY2                        0xCDEF89AB



void FLASH_UNLOCK(void);                        //разблокируем память
void FLASH_LOCK(void);                          //блокируем память
void FLASH_PAGE_ERASE(uint32_t);                //чистим страницу
void FLASH_WRITE(uint32_t, uint16_t);           //закинем данные
uint32_t FLASH_RECEIVE(uint32_t);               //читаем из памяти (только блоками по 32 бита)

void FLASH_UNLOCK(void)
{
   FLASH->KEYR = KEY1;
   FLASH->KEYR = KEY2;
    
   FLASH->ACR=FLASH_ACR_LATENCY_2|FLASH_ACR_PRFTBE;
    
   while(FLASH->CR & FLASH_CR_LOCK);
}

void FLASH_PAGE_ERASE(uint32_t addr)
{
   FLASH->CR|= FLASH_CR_PER;                     //стираем только одну страницу
   FLASH->AR = addr;                             //младший адрес этой страницы
   FLASH->CR|= FLASH_CR_STRT;                    //стираем 
   while(!(FLASH->SR & FLASH_SR_EOP));           //ждём завершения 
   FLASH->CR &= ~FLASH_CR_PER;                   //убираем бит стирания одной страницы
}

void FLASH_LOCK(void)
{
    FLASH->CR|=FLASH_CR_LOCK;
    FLASH->ACR&=~(FLASH_ACR_LATENCY_2|FLASH_ACR_PRFTBE);
}

uint32_t FLASH_RECEIVE(uint32_t addr)
{
      return (*(__IO uint32_t*) addr);
}

void FLASH_WRITE(uint32_t addr, uint16_t data)
{
  /*ЗАПИСЬ ИДЁТ ТОЛЬКО ПАКЕТАМИ ПО 16БИТ*/
    
  FLASH->CR |= FLASH_CR_PG;                     //разрешаем запись во флэш
 
  *(__IO uint16_t*)addr = data;                 //льём данные
  while(!(FLASH->SR & FLASH_SR_EOP));           //ждём завершения 
  
  FLASH->CR &= ~(FLASH_CR_PG);                  //запрещаем заливку во флэш
}


