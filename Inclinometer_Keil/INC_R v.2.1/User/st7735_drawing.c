#include "stm32f10x.h"                  //Device header
#include "math.h"

#define M_PI                    3.14159265358979323846          //потому что константа)))

/*ОПИСАНИЕ ФУНКЦИЙ:

* DRAW_AREA(uint8_t start_x,uint8_t start_y,uint8_t len_x,uint8_t len_y)
  ОПРЕДЕЛЯЕТ ОБЛАСТЬ ОТРИСОВКИ ТЕКУЩЕГО ИЗОБРАЖЕНИЯ, НА ВХОД ПРИНИМАЕТ СТАРТОВЫЕ КООРДИНАТЫ ПО X И Y,
  ДЛИНУ СТОРОН ОТРИСОВКИ ПО X И Y.

* DRAW_COLOR(uint32_t color_selection)
  РИСУЕТ ТЕКУЩИЙ ПИКСЕЛЬ ЗАДАННЫМ ЦВЕТОМ, ЦВЕТА ЗАДАЮТСЯ ПАЛИТРОЙ RGB(8,8,8)

* DRAW_IMAGE(uint8_t start_x,uint8_t start_y,uint8_t img, uint32_t color1, uint32_t color2)
  ОТРИСОВЫВАЕТ КАРТИНКУ, ЗАДАННУЮ МАССИВОМ ИЗ images.h МАССИВОМ img, НА ВХОД ПОДАЁМ НАЧАЛЬНУЮ ТОЧКУ КАРТИНКИ (x,y)
  И ДВА ЦВЕТА В 32-ХБИТНОМ ФОРМАТЕ (8,8,8)

* DRAW_RECT(uint8_t start_x,uint8_t start_y,uint8_t len_x,uint8_t len_y,uint32_t color)
  ОТРИСОВКА ПРЯМОУГОЛЬНОЙ ОБЛАСТИ НУЖНОГО ЦВЕТА(8,8,8) ФОРМАТА UINT32, НА ВХОД ПОДАЁМ НАЧАЛЬНУЮ ТОЧКУ КАРТИНКИ (x,y), 
  ЕЁ РАЗМЕРНОСТЬ (dx,dy)

* SEND_DIGITS_ON_DISP (uint8_t start_x, uint8_t start_y, int16_t on_conv,uint8_t point, uint32_t color1, uint32_t color2)
  ПРЕОБРАЗУЕТ ВХОДНОЕ ЧИСЛОВОЕ ЗНАЧЕНИЕ В ГРАФИЧЕСКОЕ ОТОБРАЖЕНИЕ, НА ВХОД ПОДАЁМ НАЧАЛЬНУЮ ТОЧКУ КАРТИНКИ (x,y), САМО ЧИСЛО,
  ПОЛОЖЕНИЕ ЗАПЯТОЙ В ЧИСЛЕ И ДВА ЦВЕТА ОТОБРАЖЕНИЯ ФОНА И ЧИСЛА
  
* DRAW_BATTERY_WIDGET(uint8_t start_x,uint8_t start_y,uint32_t color1, uint32_t color2, uint32_t u_bat)
  ИНДИКАТОР ЗАРЯДА БАТАРЕИ, НА ВХОД ПОДАЁМ НАЧАЛЬНУЮ ТОЧКУ КАРТИНКИ (x,y), ДВА ЦВЕТА ОТОБРАЖЕНИЯ ФОНА И ЧИСЛА, НА ВХОД 
  ПОДАЁМ ЗНАЧЕНИЕ С АЦП.
  
* DISP_REFRESH(uint8_t) ЗАЛИВАЕТ ВСЁ КАРТИНКОЙ ПО УМОЛЧАНИЮ

* DRAW_GRAPH_LVL (uint8_t start_x,uint8_t start_y, double pitch,double roll,uint32_t color1,uint32_t color2,
                  uint32_t color3,uint32_t color4,uint32_t color5)
  ОТОБРАЖАЕТ МИШЕНЬ С ИЗОБРАЖЕНИМ ПУЗЫРЬКА УРОВНЯ И ОТКЛОНЕНИЕМ ОТ ОСЕЙ, НА ВХОД ПРИНИМАЕТ СТАРТОВЫЕ КООРДИНАТЫ X И Y, 
  ЗНАЧЕНИЯ ОТКЛОНЕНИЙ И 5 ЦВЕТОВ ОТОБРАЖЕНЯ,1 - БАЗОВЫЙ, 2 - ЦВЕТ ЛИНИЙ, 3 - ВНУТРЕННЕГО КРУГА, 4 - ВНЕШНЕГО КРУГА, 
  5 - ЦВЕТ ПУЗЫРЬКА
  
* DIGITS_ON_DISP(float digi_1, float digi_2)
  ВЫВОДИТ НА ДИСПЛЕЙ ЗНАЧЕНИЯ digi_1 и digi_2 В СТОЛБИК
  
*/


void DRAW_AREA(uint8_t, uint8_t, uint8_t, uint8_t);                               //Определяем область, в которой будет отрисовка
void DRAW_COLOR(uint32_t);                                                        //Передаём цвет текущего пикселя
void DRAW_RECT(uint8_t,uint8_t,uint8_t,uint8_t,uint32_t);                         //Рисуем залитую область или линию
void DRAW_IMAGE(uint8_t,uint8_t,const uint32_t *,uint32_t,uint32_t);                    //Выводим картинку
void SEND_DIGITS_ON_DISP (uint8_t,uint8_t,int16_t ,uint8_t,uint32_t,uint32_t);    //Выводим полученные данные
void DRAW_BATTERY_WIDGET(uint8_t,uint8_t,uint32_t,uint32_t,uint32_t);             //Отрисовка батарейки и заряда
void DISP_REFRESH(void);                                                          //отрисовка стандартной маски дисплея
void DRAW_GRAPH_LVL (uint8_t,uint8_t,double,double,uint32_t,
                                uint32_t,uint32_t,uint32_t,uint32_t);             //Отображение мишени с пузырьком
void DIGITS_ON_DISP(float , float);                                               //Вывести цифровые значения


void DRAW_AREA(uint8_t start_x,uint8_t start_y,uint8_t len_x,uint8_t len_y)
{
    uint8_t stop_y=len_y-1+start_y;
    uint8_t stop_x=len_x-1+start_x;

    CS_LOW;
    SS7735_SEND_COM(CASET);
    SS7735_SEND_DAT(0);
    SS7735_SEND_DAT(start_y);
    SS7735_SEND_DAT(0);
    SS7735_SEND_DAT(stop_y);

    SS7735_SEND_COM(RASET);
    SS7735_SEND_DAT(0);
    SS7735_SEND_DAT(start_x);
    SS7735_SEND_DAT(0);
    SS7735_SEND_DAT(stop_x);
    CS_HIGH;  
}

void DRAW_COLOR(uint32_t color_selection)
{
    uint8_t current_color=0;       
    current_color=(uint8_t)(color_selection>>16);
    SS7735_SEND_DAT(current_color);
    current_color=(uint8_t)(color_selection>>8);
    SS7735_SEND_DAT(current_color);
    current_color=(uint8_t)color_selection;
    SS7735_SEND_DAT(current_color);
}

void DRAW_RECT(uint8_t start_x,uint8_t start_y,uint8_t len_x,uint8_t len_y,uint32_t color)
{
    DRAW_AREA(start_x,start_y,len_x,len_y);
    CS_LOW;
    SS7735_SEND_COM(RAMWR);
 
    for(uint16_t i=len_x*len_y;i>0;i--)
    {
        DRAW_COLOR(color);
    }
    CS_HIGH;
}
void DISP_REFRESH(void)
{
   DRAW_RECT(0,0,160,128,current.background); //заливаем фон 
   DRAW_RECT(0,0,160,14,current.statusbar); //горизонтальная линия верхней панели
}


void DRAW_IMAGE(uint8_t start_x,uint8_t start_y,const uint32_t img[], uint32_t color1, uint32_t color2)
{
    
    DRAW_AREA(start_x,start_y,img[0],img[1]);
    
    CS_LOW;
    SS7735_SEND_COM(RAMWR);
    for(uint16_t j=0;j<img[0];j++)
    {
       for(int i=0;i<img[1];i++)
       {
           uint32_t current_pos=img[0]*i+j;                                             //развёртка изображения в столбик
           uint8_t current_pixel=(img[current_pos/32+2]>>(31-(current_pos)%32))&0b1;    
           
           switch  (current_pixel)
           {
               case 0: DRAW_COLOR(color1);break;
               case 1: DRAW_COLOR(color2);break;
               default: DRAW_COLOR(color1);
           }    
       }
       
    }
    CS_HIGH;
    
}
    
void DRAW_BATTERY_WIDGET(uint8_t start_x,uint8_t start_y,uint32_t color1, uint32_t color2, uint32_t u_bat)
{
    //Настройка: верхний диапазон 3600, нижний - 3150, рабочая область 450 единиц
    
    uint16_t u_bat_ofst=3150;  //убираем нижние значения      
    uint16_t voltage_on_disp; 
    if(u_bat<=u_bat_ofst)
    {
        voltage_on_disp=0;
    }
    else
    {
        voltage_on_disp = (u_bat-u_bat_ofst)/4.5-2; //2кой убираем нелинейность АЦП (мёртвую зону)
    }
    // voltage_on_disp=35;   //проверка отображения в обход АЦП
    
    DRAW_IMAGE(start_x,start_y,battery,color1,color2);
    
    if(voltage_on_disp>=80)
    {
        DRAW_RECT(start_x+8,start_y+2,2,3,color2);
    }
    if(voltage_on_disp>=50)
    {
        DRAW_RECT(start_x+5,start_y+2,2,3,color2);
    }
    if(voltage_on_disp>=20)
    {
        DRAW_RECT(start_x+2,start_y+2,2,3,color2);
    }
   
    
    uint8_t offset_x=0;
    if(voltage_on_disp>=100)
    {
        DRAW_IMAGE(start_x+15,start_y,small_one,color1,color2);
        DRAW_IMAGE(start_x+20,start_y,small_zero,color1,color2);
        DRAW_IMAGE(start_x+25,start_y,small_zero,color1,color2);
    }
    else
    {
       
        for(int i=10;i>=1;i=i/10)
        {
            uint8_t digit;
            digit=(uint8_t)((voltage_on_disp/i)%10); 
            
            switch (digit)
            {
                case 0: DRAW_IMAGE(start_x+15+offset_x,start_y,small_zero,color1,color2);break;
                case 1: DRAW_IMAGE(start_x+15+offset_x,start_y,small_one,color1,color2);break;
                case 2: DRAW_IMAGE(start_x+15+offset_x,start_y,small_two,color1,color2);break;
                case 3: DRAW_IMAGE(start_x+15+offset_x,start_y,small_three,color1,color2);break;
                case 4: DRAW_IMAGE(start_x+15+offset_x,start_y,small_four,color1,color2);break;
                case 5: DRAW_IMAGE(start_x+15+offset_x,start_y,small_five,color1,color2);break;
                case 6: DRAW_IMAGE(start_x+15+offset_x,start_y,small_six,color1,color2);break;
                case 7: DRAW_IMAGE(start_x+15+offset_x,start_y,small_seven,color1,color2);break;
                case 8: DRAW_IMAGE(start_x+15+offset_x,start_y,small_eight,color1,color2);break;
                case 9: DRAW_IMAGE(start_x+15+offset_x,start_y,small_nine,color1,color2);break;            
            } 
            offset_x+=5;
            
            DRAW_RECT(start_x+20+offset_x,start_y,7,10,color1);
    } 
        
        
        
    }        
    
}


void SEND_DIGITS_ON_DISP (uint8_t start_x, uint8_t start_y, int16_t on_conv,uint8_t point, uint32_t color1, uint32_t color2)
 {
    
    uint8_t digit;
     
    if(on_conv<0)
    {
        on_conv=-on_conv;
    }
    
    for (int i=1000;i>=1;i=i/10)
    {
        
        if (point==0)
        {
            if (i==1)
            {
                 DRAW_IMAGE(start_x,(start_y+12),large_dot,color1,color2);
                 start_x=start_x+5;
            }
        }
        
        if (point==1)
        {
            if (i==1)
            {
                 DRAW_IMAGE(start_x,(start_y+12),large_dot,color1,color2);
                 start_x=start_x+5;
            }
        }
        digit=(uint8_t)((on_conv/i)%10); 
    
        switch (digit)
        {
            case 0: DRAW_IMAGE(start_x,start_y,large_zero,color1,color2);break;
            case 1: DRAW_IMAGE(start_x,start_y,large_one,color1,color2);break;
            case 2: DRAW_IMAGE(start_x,start_y,large_two,color1,color2);break;
            case 3: DRAW_IMAGE(start_x,start_y,large_three,color1,color2);break;
            case 4: DRAW_IMAGE(start_x,start_y,large_four,color1,color2);break;
            case 5: DRAW_IMAGE(start_x,start_y,large_five,color1,color2);break;
            case 6: DRAW_IMAGE(start_x,start_y,large_six,color1,color2);break;
            case 7: DRAW_IMAGE(start_x,start_y,large_seven,color1,color2);break;
            case 8: DRAW_IMAGE(start_x,start_y,large_eight,color1,color2);break;
            case 9: DRAW_IMAGE(start_x,start_y,large_nine,color1,color2);break;            
        }
        
        start_x=start_x+10;    
    }
    
}
 







void DRAW_GRAPH_LVL (uint8_t start_x,uint8_t start_y, double pitch,double roll,uint32_t color1,uint32_t color2,
                     uint32_t color3,uint32_t color4,uint32_t color5)
{
   int8_t offset_x=0;                                                  //смещение пузырька по оси x
   int8_t offset_y=0;                                                  //смещение пузырька по оси y
   double angle_b=0;                                                   //угол смещения пузырька  
   double modul_b=0;                                                   //модуль смещения пузырька
   int16_t bubble_diameter=13;                                         //диаметр пузырька в пикселях
   int16_t target_outer_radius=32;                                     //рабочий радиус мишени (в пикселях)
   int16_t field_range=77;                                             //разрешение поля под пузырёк (сторона квадрата)


    
   DRAW_AREA(start_x,start_y,field_range,field_range);
   angle_b=atan2(roll,pitch);
   modul_b=sqrt(roll*roll+pitch*pitch);
   if(modul_b>target_outer_radius-bubble_diameter/2+1)
   {modul_b=target_outer_radius-bubble_diameter/2+1;}
   offset_x=(int8_t)(modul_b*cos(angle_b));
   offset_y=(int8_t)(modul_b*sin(angle_b));
        
   CS_LOW;
   SS7735_SEND_COM(RAMWR);
   for(uint16_t j=0;j<field_range;j++)
   {
     for(int i=0;i<field_range;i++)
     {
        uint8_t current_pixel_t;
        uint8_t current_pixel_b;
        uint8_t current_pixel;
        if((i>=offset_y+target_outer_radius)&(i<offset_y+target_outer_radius+bubble_diameter)&
            (j>=offset_x+target_outer_radius)&(j<offset_x+target_outer_radius+bubble_diameter))
        {
            current_pixel_t=bubble[(i-(target_outer_radius+offset_y))+bubble_diameter*(j-(target_outer_radius+offset_x))];
            current_pixel_b=target[field_range*i+j];
        }
        else
        {
            current_pixel_b=target[field_range*i+j];
            current_pixel_t=0;
        }
        
        if(current_pixel_t<current_pixel_b)
        {current_pixel=current_pixel_b;}
        else
        {current_pixel=current_pixel_t;}
        
            
        switch  (current_pixel)
        {
            case 0:  DRAW_COLOR(color1);break;
            case 1:  DRAW_COLOR(color2);break;
            case 2:  DRAW_COLOR(color3);break;
            case 3:  DRAW_COLOR(color4);break;
            case 4:  DRAW_COLOR(color5);break;
            default: DRAW_COLOR(color1);
        }    
     }
       
   }
   CS_HIGH;
    
}

void DIGITS_ON_DISP(float digi_1, float digi_2)
{
   
   SEND_DIGITS_ON_DISP (8,48,(int16_t)(digi_1*10),1,
                                     current.background,current.big_digits);     //отправляем цифровые значения по оси x
   SEND_DIGITS_ON_DISP (8,89,(int16_t)(digi_2*10),1,
                                     current.background,current.big_digits);     //и по оси y, смена значений происходит в 
                                                                                 //блоке main, в зависимости от выбраного режима
   DRAW_IMAGE(8,72,y_axis,current.background,current.statusbar);                            //надпись
   DRAW_IMAGE(8,31,x_axis,current.background,current.statusbar);                            //надпись
    
  
   
   //В ЗАВИСИМОСТИ ОТ ЗНАКА ЗНАЧЕНИЙ ПО ОСЯМ X И Y МЕНЯЕТСЯ ПОЛОЖЕНИЕ СТРЕЛОК 
    
   if(pitch<0)
   {DRAW_IMAGE(48,32,right_arrow,current.background,current.statusbar);}                   
   else
   {DRAW_IMAGE(48,32,left_arrow,current.background,current.statusbar);}
   
   if(roll<0)
   {DRAW_IMAGE(48,72,down_arrow,current.background,current.statusbar);}
   else
   {DRAW_IMAGE(48,72,up_arrow,current.background,current.statusbar);}  
}

