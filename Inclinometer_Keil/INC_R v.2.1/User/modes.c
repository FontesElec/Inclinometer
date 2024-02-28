#include "stm32f10x.h"                  //Device header

void ANGLES(void);
void SLOPES(void);
void WAIT(void);


void ANGLES(void)
{
    DRAW_GRAPH_LVL (75,30,pitch,roll,current.background,current.statusbar,
                                    current.big_digits,current.statusbar,current.ball);   //нарисовали в картиночке
    DIGITS_ON_DISP(pitch,roll);                                                           //вывели в циферках
    DRAW_IMAGE(4,4,angle_sign,current.statusbar,current.background);                      //надпись angle в верхнем поле
    DRAW_IMAGE(56,48,grad_sign,current.background,current.big_digits);                    //после цифры ставим значок градусов        
    DRAW_IMAGE(56,89,grad_sign,current.background,current.big_digits);
}

void SLOPES(void)
{
    float pitch_slope, roll_slope;                                                        //пересчёт в уклон
    pitch_slope=tan(M_PI*pitch/180)*100;
    roll_slope=tan(M_PI*roll/180)*100;
    DRAW_GRAPH_LVL (75,30,pitch,roll,current.background,current.statusbar,
                                    current.big_digits,current.statusbar,current.ball);   //нарисовали в картиночке
    DIGITS_ON_DISP(pitch_slope, roll_slope);                                              //вывели в циферках
    DRAW_IMAGE(4,4,slope_sign,current.statusbar,current.background);                      //надпись slope в верхнем поле 
    DRAW_IMAGE(56,48,percent_sign,current.background,current.big_digits);                 //знак процента (знак уклона)
    DRAW_IMAGE(56,89,percent_sign,current.background,current.big_digits);
}

void WAIT(void)
{
    
     while(mode==2){}
   
   
}

