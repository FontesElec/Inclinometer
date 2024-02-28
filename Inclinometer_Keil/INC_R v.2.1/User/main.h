#include "stm32f10x.h"                  //Device header

#define M_PI                            3.14159265358979323846          //потому что константа)))
#define F_BATTERY_REFR                  200                             //частота обновления виджета батареи 

void    DELAY(int);                             //простая функция задержки  
void    RECEIVING_n_FILTERING_DATA(uint32_t);   //обработка данных с датчика
void    ANGLES_CALC(void);                      //преобразование углов
float   SIMPLE_KALMAN(float,uint8_t);           //фильтр Калмана

float N;                                    //модуль ускорения
float Nx, Ny, Nz;                           //значения ускорений с датчика
float pitch,roll;                           //углы отклонения от горизонта
float pitch_offs,roll_offs;                 //смещение углов отклонения от горизонта
float zero_x,zero_y,zero_z;                 //параметры смещения
const float err_measure = 17000;             //примерный шум измерений
float q = 0.001;                             //скорость изменения значений 0.001-1
float calib_const[6];                       //калибровочная таблица значений


struct theme                                //цветовая тема
{
    uint32_t background;
    uint32_t statusbar;
    uint32_t big_digits;
    uint32_t ball;
};

struct theme current = {0xD0AA60,0xA90400,0x5E0000,0xFF1F00};

uint8_t hold=0;                             //теперь сюда льём номер режима из которого вышли в hold
uint8_t filt_reset=1;                       //запуск сброса фильтра
uint8_t zero_flag=0;                        //флаг отрисовки надписи ZERO
uint8_t mode=0;                             //параметр текущего режима
uint8_t battery_ref_freq=F_BATTERY_REFR;    //счётчик периода обновления данных о заряде
