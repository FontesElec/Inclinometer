#include "stm32f10x.h"                  //Device header
void CALIBRATION(void);
void LOAD_CALIB_CONST(void);
void FLASH_WRITE_FLOAT(uint32_t,float);


void CALIBRATION(void){
   
    uint32_t delay_p=5000000;
    float calib_c[6];                                       
    uint8_t ok_pressed=0;
    uint8_t cancel_pressed=0;

    Nx=0;
    Ny=0;
    Nz=16384;
    
    DRAW_RECT(0,0,160,128,current.background);                                      //заливаем фон 
    DRAW_IMAGE(15,10,calibration_sign,current.background,current.statusbar);
    
    while((GPIOB->IDR)&GPIO_IDR_IDR5);                                              //ожидаем, когда отожмут кнопку
    DELAY(delay_p);
    
    //отрисовка первой картинки 
    DRAW_IMAGE(15,25,put_sign,current.background,current.statusbar);    
    DRAW_IMAGE(35,25,flat_sign,current.background,current.statusbar); 
    DRAW_IMAGE(136,10,cancel_sign,current.background,current.statusbar);
    DRAW_IMAGE(146,11,right_arrow,current.background,current.statusbar);  
    DRAW_IMAGE(15,35,and_press_sign,current.background,current.statusbar); 
    DRAW_IMAGE(68,35,ok_sign,current.background,current.statusbar);    
    DRAW_IMAGE(30,55,calib1,current.background,current.statusbar); 
    DRAW_IMAGE(134,111,ok_sign,current.background,current.statusbar);  
    DRAW_IMAGE(146,112,right_arrow,current.background,current.statusbar);  
    
    //ожидание реакции пользователя
    while(!ok_pressed)
    {
        if((GPIOB->IDR)&GPIO_IDR_IDR8)                                                    //нажата кнопка POWER
        {
           cancel_pressed=1;
           ok_pressed=1;
        }
        if(((GPIOB->IDR)&GPIO_IDR_IDR5))                                                  //нажата кнопка mode
        {
            ok_pressed=1;  
        }    
    }
    
    //обработка по результатам реакции
    if(!cancel_pressed){
        ok_pressed=0;
        DRAW_RECT(134,111,26,7,current.background);                                        //если продолжили, то надпись wait
        DRAW_RECT(134,10,26,7,current.background); 
        DRAW_RECT(15,25,70,20,current.background);
        DRAW_IMAGE(15,25,wait_sign,current.background,current.statusbar);
/*
        for(int i=1000000;i>0;i--)
        {
            Nx+= ((-(float)(ADXL345_SPI_RECEIVE_16BIT(DATAX0))) - Nx) * k_filter;
            Ny+= ((-(float)(ADXL345_SPI_RECEIVE_16BIT(DATAY0))) - Ny) * k_filter;
        }
 */     
      
        RECEIVING_n_FILTERING_DATA(200000);                                                //запуск обработки сигнала
        calib_c[0]=-Nx;
        calib_c[1]=-Ny;
        calib_c[2]=16384-Nz;
        Nx=0;
        Ny=16384;
        Nz=0;
        
        DRAW_RECT(15,25,30,10,current.background);
        DRAW_IMAGE(15,25,done_sign,current.background,current.statusbar);                  //отчитались о завершении
        DELAY(delay_p);
        DRAW_RECT(0,0,160,128,current.background);                                          //заливаем фон 
        
          
        
        //вторая часть калибровки
        DRAW_IMAGE(15,10,calibration_sign,current.background,current.statusbar);        
        DELAY(delay_p);
        
        //вновь отрисовываем картинку
        DRAW_IMAGE(15,25,put_sign,current.background,current.statusbar); 
        DRAW_IMAGE(35,25,on_the_edge_sign,current.background,current.statusbar); 
        DRAW_IMAGE(136,10,cancel_sign,current.background,current.statusbar);
        DRAW_IMAGE(146,11,right_arrow,current.background,current.statusbar);  
        DRAW_IMAGE(15,35,and_press_sign,current.background,current.statusbar); 
        DRAW_IMAGE(68,35,ok_sign,current.background,current.statusbar);    
        DRAW_IMAGE(30,46,calib2,current.background,current.statusbar); 
        DRAW_IMAGE(134,111,ok_sign,current.background,current.statusbar);  
        DRAW_IMAGE(146,112,right_arrow,current.background,current.statusbar);  
        
        //вновь ждём реакции пользователя
        while(!ok_pressed)
        {
            if((GPIOB->IDR)&GPIO_IDR_IDR8)                                                    //нажата кнопка POWER
            {
               cancel_pressed=1;
               ok_pressed=1;
            }
            if(((GPIOB->IDR)&GPIO_IDR_IDR5))                                                  //нажата кнопка mode
            {
                ok_pressed=1;  
            }    
        }
        
        if(!cancel_pressed){
            
            DRAW_RECT(134,111,26,7,current.background);
            DRAW_RECT(134,10,26,7,current.background); 
            DRAW_RECT(15,25,90,20,current.background);
            DRAW_IMAGE(15,25,wait_sign,current.background,current.statusbar); 

            RECEIVING_n_FILTERING_DATA(200000);
           
            calib_c[3]=-Nx;
            calib_c[4]=16384-Ny;
            calib_c[5]=-Nz;
            
         
            
            DRAW_RECT(15,25,30,10,current.background);
            DRAW_IMAGE(15,25,done_sign,current.background,current.statusbar);
            DELAY(delay_p);
            
            //оставшиеся ячейки пока дописываем копируя известные значения
            //calib_c[2]=calib_c[5];                          //берём старое значение -Nz
            //calib_c[4]=calib_c[1];                          //берём старое значение -Ny
            /*
            for(int i=0;i<6;i++)
            {
              calib_c[i]=i*0.2;  
            }
            */
            //зашиваем новые параметры
            FLASH_UNLOCK();
            FLASH_PAGE_ERASE(0x8007C00);    
            for(int i=0;i<6;i++)
            {
               FLASH_WRITE_FLOAT(0x8007C00+4*i,calib_c[i]) ;                
            }
            FLASH_LOCK();
            
        }       
    
    }
   
    //завершение, затираем картинку
    DRAW_RECT(0,0,160,128,current.background); //заливаем фон 
    
    /*
    FLASH_UNLOCK();
    FLASH_PAGE_ERASE(0x8007C00);    
    RECEIVING_n_FILTERING_DATA(100000);    
    FLASH_WRITE(0x8007C00,-Nx); 
    FLASH_WRITE(0x8007C04,-Ny);
    FLASH_LOCK();
    */
/*
    float c_zero_x=-Nx;
    float c_zero_y=-Ny;
    FLASH_UNLOCK();
    FLASH_PAGE_ERASE(0x8007C00); 
    FLASH_WRITE(0x8007C00, *((uint16_t*)&c_zero_x));           //закинем данные
    FLASH_WRITE(0x8007C02, *(((uint16_t*)(&c_zero_x))+1));           //закинем данные
    FLASH_WRITE(0x8007C04, *((uint16_t*)&c_zero_y));           //закинем данные
    FLASH_WRITE(0x8007C06, *(((uint16_t*)(&c_zero_y))+1));           //закинем данные
    FLASH_LOCK();
    
*/    
    
}

void LOAD_CALIB_CONST(void)
{
    uint32_t temp;
    for(int i=0;i<6;i++)
    {
       temp=FLASH_RECEIVE(0x8007C00+i*4);
       calib_const[i]=*(float*)&temp;
    }
}

void FLASH_WRITE_FLOAT(uint32_t adr,float param)
{
    FLASH_WRITE(adr, *((uint16_t*)&param));                   //закинем данные
    FLASH_WRITE(adr+2, *(((uint16_t*)(&param))+1));           //смещение по адресу на 16бит дописываем переменную
    
}
    



/*
   float temp=34.79;
   //uint32_t temp=0x420b3000;
   uint16_t* ptr;
   ptr= (uint16_t*)  &temp;
  FLASH_UNLOCK();
   FLASH_PAGE_ERASE(0x8007C00); 
   FLASH_WRITE(0x8007C00, *ptr);           //закинем данные
   FLASH_WRITE(0x8007C02, *(ptr+1));           //закинем данные
   //FLASH_WRITE(0x8007C02, (*(uint16_t*)&temp));
   //FLASH_WRITE(0x8007C02, (*((uint16_t*)(&temp+1))));
   FLASH_LOCK();
   
    uint32_t temp=FLASH_RECEIVE(0x8007C00);
    //uint32_t temp=0x420B28F6;
    pitch=*(float*)&temp;                                                                 //чтение только блоками по 32 бита
    */

