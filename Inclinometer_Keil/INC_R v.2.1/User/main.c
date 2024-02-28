/*
Автор: Чудаков Максим 
e-mail: fontes35@mail.ru
Дата создания: 21.02.2022

Инклинометр на базе микроконтроллера STM32F103C8T6 (CORE M3), TFT дисплея ST7735S (связь SPI1), 
датчика линейных ускорений ADXL345 (связь по SPI2)

Версия: 2.1

Изменения:

* Настроен датчик ADXL345, вместо 10bit режима стал full range в DATA_FORMAT регистре, теперь 1g соответствует 16384; 
* Исправлена проблема с снижением скорости работы программы после записи во флэш;


* ПРОГРАММА НЕ РАБОТАЕТ С ПЕРВОЙ СТАБИЛЬНОЙ ВЕРСИЕЙ ПЛАТЫ: НА ВХОДЕ ДРУГОЙ ТИП ВЫКЛЮЧЕНИЯ, АЦП НАСТРОЕН НА РАБОТУ С LI-PO АКБ
*/

#include "stm32f10x.h"                  //Device header
#include "math.h"                       //Математика
#include "main.h"
#include "stm32f10xx_clock_settings.c"  //Настройки тактирования от внутреннего кварца
#include "adc_stm32f103.c"              //Инициализация работы АЦП
#include "buttons.c"                    //Инициализация портов под кнопки
#include "flash_stm32f103.c"            //Запись и чтение данных из флэш памяти
#include "adxl345.h"                    //Макросы регистров и служебные макросы работы переферии SPI ADXL345
#include "adxl345.c"                    //Инициализация работы и протоколы взаимодействия (по шине SPI2) adxl345
#include "images.h"                     //Сюда льём массивы изображений
#include "color_style.c"                //Цветовое оформление различных элементов
#include "st7735s reg_map.h"            //Адреса регистров
#include "st7735s.h"                    //Служебные макросы и описание функций
#include "st7735_init.c"                //Файл инициализации дисплея и работа (по шине SPI1)
#include "st7735_drawing.c"             //Функции отрисовки
#include "power_mgr.c"                  //Работа с системой питания (опрос заряда, выключение)
#include "modes.c"                      //Описание режимов работы
//#include "pwm_stm32f103.c"              //Настройки ШИМ
#include "accel_calib.c"                //Калибровка акселерометра




int main(void)
{
   CLK_INIT();                         //Настройка тактирования
   ADC_INIT();                         //Настройка и запуск АЦП (вольтметр батарейки)
   KEY_INIT();                         //Настройка кнопок управления    
   ADXL345_INIT();                     //Запуск шины SPI2 и настройка работы датчика
   SS7735_INIT();                      //Запуск шины SPI1 и настройка работы с дисплеем
   DISP_REFRESH();                     //Очистить дисплей
  
   if(((GPIOB->IDR)&GPIO_IDR_IDR5)==GPIO_IDR_IDR5)                              //если нажата кнопка mode
   {
       CALIBRATION();                                                           //В режим калибровки
   }
    
   LOAD_CALIB_CONST();                 //Подгружаем с flash калибровочные константы    
   EXTI_CONFIG();                      //Настройка и запуск прерываний по кнопкам
   
   //DRAW_IMAGE(60,60,test,(current.background),(current.statusbar));            //тестовая картинка(вертикальные полосы 
   DRAW_IMAGE(60,60,hello_sign,current.background,current.statusbar);           //приветствие
   while(((GPIOB->IDR)&GPIO_IDR_IDR8)){};                                       //ждём когда отпустят кнопку питания
   DELAY(7000000);                                                              //дать время для подгрузки АЦП
   DISP_REFRESH();                                                              //Очистить дисплей
      
   while(1)
   {
      //=======================================ОБНОВЛЕНИЕ ОТОБРАЖЕНИЯ БАТАРЕЙКИ============================================
              
       if(battery_ref_freq==F_BATTERY_REFR)                                     //частота обновления заряда батареи
       {
           BATT_CHECK();
           battery_ref_freq=0;           
       }
       ++battery_ref_freq;
 
      //-------------------------------------------------------------------------------------------------------------------    
      ANGLES_CALC();                                                            //посчитали значения отклонения         
       
      //===================ИЗМЕНЕНИЕ РЕЖИМА ОТОБРАЖЕНИЯ НА ДИСПЛЕЕ В ЗАВИСИМОСТИ ОТ ТЕКУЩЕГО РЕЖИМА======================== 
       
       switch(mode)
       {
           case 0: ANGLES();        break;
           case 1: SLOPES();        break;
           case 2: WAIT();          break;
           default:
           {
               ANGLES();
           }
       }
       
      //--------------------------------------------------------------------------------------------------------------------
    }    
}


void ANGLES_CALC(void)
{
   

    RECEIVING_n_FILTERING_DATA(1000);
    
    Nx=Nx+zero_x;
    Ny=Ny+zero_y;
    Nz=Nz+zero_z;
    
    N=sqrt(Nx*Nx+Ny*Ny+Nz*Nz);
    
    if((Nz*Nz>Ny*Ny)&(Nz*Nz>Nx*Nx))                                 //если положен плашмя
    {     
      zero_x=calib_const[0];
      zero_y=calib_const[1];
      zero_z=calib_const[2];
      pitch=180*atan2(-Nx,sqrt((double)((Ny*Ny)+(Nz*Nz))))/M_PI-pitch_offs;
      roll=180*atan2(-Ny,sqrt((double)((Nz*Nz)+(Nx*Nx))))/M_PI-roll_offs;
    }   
    else if(Ny>=0)                                                      //вертикальное положение
    {
      zero_x=calib_const[3];
      zero_y=calib_const[4];
      zero_z=calib_const[5];
      pitch=180*atan2(-Nx,sqrt((double)((Ny*Ny)+(Nz*Nz))))/M_PI-pitch_offs;
      roll=180*atan2(Nz,sqrt((double)((Ny*Ny)+(Nx*Nx))))/M_PI-roll_offs;
    }
    else
    {
      zero_x=calib_const[3];
      zero_y=calib_const[4];
      zero_z=calib_const[5];
      pitch=-180*atan2(Nx,-sqrt((double)((Ny*Ny)+(Nz*Nz))))/M_PI-pitch_offs;
      roll=180*atan2(-Nz,sqrt((double)((Ny*Ny)+(Nx*Nx))))/M_PI-roll_offs;
    }
    
 
    
}

void EXTI9_5_IRQHandler(void)
{
    uint32_t delay_p=4000000;
                                              
    if((EXTI->PR)&EXTI_PR_PR8)                                                          //была нажата кнопка POWER
    {
       POWER_OFF();
    }    
    
    else if((EXTI->PR)&EXTI_PR_PR5)                                                     //была нажата кнопка mode
    {
        mode++;
        if (mode>1)
        {mode=0;}
    }
    
    else if((EXTI->PR)&EXTI_PR_PR7)                                                     //была нажата кнопка hold
    {
        if(mode==2)
        {
            DRAW_RECT(50,4,25,7,current.statusbar);                                     //заливаем фон 
            mode=hold;
            //Возврат маски прерываний для mode и zero
            EXTI->IMR|=EXTI_IMR_MR5|EXTI_IMR_MR6;            
        }
        else 
        {          
            DRAW_IMAGE(50,4,hold_sign,current.statusbar,current.background);            //надпись hold
            hold=mode;                                                                  //запомним режим, из которого выходили                                                                        
            mode=2;                                                                     //в режим ожидания
            //отключим реакцию на кнопки hold и zero
            EXTI->IMR&=~(EXTI_IMR_MR5|EXTI_IMR_MR6);
            
        }
    }
    
    else if((EXTI->PR)&EXTI_PR_PR6)                                                     //нажата кнопка zero
    {
       
        if(zero_flag)
        {
            //сбрасываем флаг и параметры смещения нуля
            zero_flag=0;
            roll_offs=0;                                              
            pitch_offs=0;
            DRAW_RECT(90,4,25,7,current.statusbar);                                     //заливаем фон 
        }
        else
        {
            roll_offs=roll+roll_offs;                                                   //считаем разницу относительно текущего нуля
            pitch_offs=pitch+pitch_offs;
            DRAW_IMAGE(90,4,zero_sign,current.statusbar,current.background);            //предупреждаем о смещении настроек нуля                                                                                   
            zero_flag=1;                                                                //взводим флаг отображения надписи
                                                                                        //нужно при смене ориентации экрана
        }        
     }
     DELAY(delay_p);
     EXTI->PR&=~(EXTI_PR_PR5&EXTI_PR_PR6&EXTI_PR_PR7);                                  //сброс флагов, иначе не выйдет из прерывания
};


float SIMPLE_KALMAN(float newVal, uint8_t axis) {
  float kalman_gain, current_estimate;
  static float err_estimate[3] = {1.0,1.0,1.0};
  static float last_estimate[3];
  kalman_gain = (float)err_estimate[axis] / (err_estimate[axis] + err_measure);
  current_estimate = last_estimate[axis] + (float)kalman_gain * (newVal - last_estimate[axis]);
  err_estimate[axis] =  (1.0 - kalman_gain) * err_estimate[axis] + fabs(last_estimate[axis] - current_estimate) * q;
  //err_estimate[axis] =  (1.0 - kalman_gain) * err_estimate[axis];
  last_estimate[axis] = current_estimate;
  return current_estimate;
}
void CUSTOM_FILTER(void)
{
    
}

void DELAY(int delay)
{
    for(;delay>0;delay--);
}

void RECEIVING_n_FILTERING_DATA(uint32_t j)
{
    for(;j>0;j--)
    {
            Nx=SIMPLE_KALMAN((float)(ADXL345_SPI_RECEIVE_16BIT(DATAX0)),0);
            Ny=SIMPLE_KALMAN((float)(ADXL345_SPI_RECEIVE_16BIT(DATAY0)),1);
            Nz=SIMPLE_KALMAN((float)(ADXL345_SPI_RECEIVE_16BIT(DATAZ0)),2);
    }   
}
