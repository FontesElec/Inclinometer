//-----------------------------ADXL345 REGISTER MAP-----------------------------

//#define     DEVID               0x00            //device ID                 
//#define     OFSX                0x1E            //x_axis offset
//#define     OFSY                0x1F            //y_axis offset
//#define     OFSZ                0x20            //z_axis offset
#define     BW_RATE             0x2C            //data rate and power mode control 
#define     POWER_CTL           0x2D            //power-saving features control 
#define     DATA_FORMAT         0x31            //data format control 
#define     DATAY0              0x32            //X-Axis Data 0 
#define     DATAY1              0x33            //X-Axis Data 1 
#define     DATAX0              0x34            //Y-Axis Data 0 
#define     DATAX1              0x35            //Y-Axis Data 1 
#define     DATAZ0              0x36            //Z-Axis Data 0 
#define     DATAZ1              0x37            //Z-Axis Data 1 
//#define     FIFO_CTL            0x38            //FIFO control 
//#define     FIFO_STATUS         0x39            //FIFO status 

//=============================ADXL345 REGISTER MAP=============================

//----------------------------ADXL345 COMMAND VALUES----------------------------

//power_ctl
#define     ADXL345_MEASURE     0b00001000      //measurement mode
#define     ADXL345_SLEEP       0b00000111      //sleep mode with 1Hz frequency of readings
//bw_rate
#define     ADXL345_REDUCE_PWR  0b00010000      //selects reduced power operation
#define     ADXL345_MAX_D_RATE  0b00001111      //Output DataRate 3200Hz 
//data_format
#define     ADXL345_D_FORMAT    0b00001100      //no self test, 4 line SPI,left-justified mode, +-2g range,full resolution



//============================ADXL345 COMMNAND VALUES===========================

//--------------------------------SERVICE DEFINES-------------------------------

#define ADXL345_CS_LOW          GPIOB->BSRR|=GPIO_BSRR_BR12;     //установка 0 на SS(PB12)
#define ADXL345_CS_HIGH         GPIOB->BSRR|=GPIO_BSRR_BS12;     //установка 1 на SS(PB12)
#define ADXL345_SPI_DR_8bit     (*(uint8_t*)&(SPI2->DR))         //отправка данных в регистр SPI в режиме 8 бит
#define ADXL345_SPI_READ        0b11000000                       //читаем данные датчика
#define ADXL345_SPI_WRITE       0                                //пишем данные в датчик

//================================SERVICE DEFINES===============================
