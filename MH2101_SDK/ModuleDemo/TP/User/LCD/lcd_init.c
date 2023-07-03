#include "mhscpu.h"
#include "lcd_init.h"
#include "delay.h"


void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
    
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_GPIO, ENABLE);    
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_RESET_PIN;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(LCD_RESET_GPIOx, &GPIO_InitStructure);
    LCD_RES_Set();
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_DC_PIN;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(LCD_DC_GPIOx, &GPIO_InitStructure);
    LCD_DC_Set();    
   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_CS_PIN;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(LCD_CS_GPIOx, &GPIO_InitStructure);
    LCD_CS_Set();
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_BL_PIN;
    GPIO_InitStructure.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(LCD_BL_GPIOx, &GPIO_InitStructure);
    LCD_BL_On();	
}

void LCD_SPI_Init(void)
{
    HSPIM_InitTypeDef HSPIM_InitStructure;
 	HSPIM_DMAInitTypeDef HSPIM_DMAInitStructure;
    
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_GPIO, ENABLE); 	
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_HSPI0 | SYSCTRL_AHBPeriph_DMA, ENABLE);  
    
    GPIO_PinRemapConfig(GPIOC, GPIO_Pin_12 | GPIO_Pin_13 |  GPIO_Pin_15, GPIO_Remap_3);
    
	HSPIM_DMAInitStructure.HSPIM_DMATransmitEnable = ENABLE;
	HSPIM_DMAInitStructure.HSPIM_DMATransmitLevel = HSPIM_DMA_TRANSMIT_LEVEL_31;	
	HSPIM_DMAInit(LCD_SPI, &HSPIM_DMAInitStructure);		
	
    HSPIM_InitStructure.HSPIM_BaudRatePrescaler = HSPIM_BAUDRATE_PRESCALER_4;
    HSPIM_InitStructure.HSPIM_CPHA = HSPIM_CPHA_2EDGE;
    HSPIM_InitStructure.HSPIM_CPOL = HSPIM_CPOL_HIGH;
    HSPIM_InitStructure.HSPIM_DivideEnable = ENABLE;
    HSPIM_InitStructure.HSPIM_FirstBit = HSPIM_FIRST_BIT_MSB;
    HSPIM_InitStructure.HSPIM_Mode = HSPIM_MODE_STANDARD;
    HSPIM_InitStructure.HSPIM_RXFIFOFullThreshold = HSPIM_RX_FIFO_FULL_THRESHOLD_5;
    HSPIM_InitStructure.HSPIM_TXFIFOEmptyThreshold = HSPIM_TX_FIFO_EMPTY_THRESHOLD_5;
    
	HSPIM_FIFOReset(LCD_SPI, HSPIM_FIFO_TX | HSPIM_FIFO_RX);
	HSPIM_Init(LCD_SPI, &HSPIM_InitStructure);
	HSPIM_Cmd(LCD_SPI, ENABLE);
	HSPIM_TransmitCmd(LCD_SPI, ENABLE);  
}


/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(uint8_t dat) 
{	
    uint16_t recv;
	
    while(RESET == HSPIM_GetFlagStatus(LCD_SPI, HSPIM_FLAG_TXE));
    HSPIM_SendData(LCD_SPI, dat);
    while (SET == HSPIM_GetFlagStatus(LCD_SPI, HSPIM_FLAG_RXE));
    recv = HSPIM_ReceiveData(LCD_SPI);
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(uint8_t dat)
{
	LCD_CS_Clr();
	LCD_Writ_Bus(dat);
    LCD_CS_Set();	
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(uint16_t dat)
{
	LCD_CS_Clr();	
	
	LCD_Writ_Bus(dat >> 8);
	LCD_Writ_Bus(dat);
	
    LCD_CS_Set();		
}


/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(uint8_t dat)
{
	LCD_CS_Clr();
	
	LCD_DC_Clr();   //写命令
	LCD_Writ_Bus(dat);
	LCD_DC_Set();   //写数据
	
    LCD_CS_Set();		
}


/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    LCD_WR_REG(0x2a);//列地址设置
    LCD_WR_DATA(x1);
    LCD_WR_DATA(x2);
    LCD_WR_REG(0x2b);//行地址设置
    LCD_WR_DATA(y1);
    LCD_WR_DATA(y2);
    LCD_WR_REG(0x2c);//储存器写
}

void LCD_Init(void)
{
	LCD_GPIO_Init();  //初始化GPIO
	LCD_SPI_Init();
    
	LCD_RES_Clr();    //复位
	mdelay(100);
	LCD_RES_Set();
	mdelay(100);
	
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11);       //Sleep out 
	mdelay(120);            //Delay 120ms 

	/************************************* Driver IC:ST7789 ******************************************/
	//--------------------------------------Display Setting------------------------------------------//
	LCD_WR_REG(0x36);
	LCD_WR_DATA8(0x70);
	LCD_WR_REG(0x3a);
	LCD_WR_DATA8(0x05);
	//--------------------------------ST7789V Frame rate setting----------------------------------//
	LCD_WR_REG(0xb2);
	LCD_WR_DATA8(0x0c);
	LCD_WR_DATA8(0x0c);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33);
	LCD_WR_REG(0xb7);
	LCD_WR_DATA8(0x35);
	//---------------------------------ST7789V Power setting--------------------------------------//
	LCD_WR_REG(0xbb);
	LCD_WR_DATA8(0x20);
	LCD_WR_REG(0xc0);
	LCD_WR_DATA8(0x2c);
	LCD_WR_REG(0xc2);
	LCD_WR_DATA8(0x01);
	LCD_WR_REG(0xc3);
	LCD_WR_DATA8(0x0b);
	LCD_WR_REG(0xc4);
	LCD_WR_DATA8(0x20);
	LCD_WR_REG(0xc6);
	LCD_WR_DATA8(0x0f);
	LCD_WR_REG(0xd0);
	LCD_WR_DATA8(0xa4);
	LCD_WR_DATA8(0xa1);
	//--------------------------------ST7789V gamma setting---------------------------------------//
	LCD_WR_REG(0xe0);
	LCD_WR_DATA8(0xd0);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x0e);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x3d);
	LCD_WR_DATA8(0x47);
	LCD_WR_DATA8(0x55);
	LCD_WR_DATA8(0x53);
	LCD_WR_DATA8(0x1a);
	LCD_WR_DATA8(0x16);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x1f);
	LCD_WR_DATA8(0x22);
	LCD_WR_REG(0xe1);
	LCD_WR_DATA8(0xd0);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x0d);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x2c);
	LCD_WR_DATA8(0x43);
	LCD_WR_DATA8(0x55);
	LCD_WR_DATA8(0x53);
	LCD_WR_DATA8(0x1e);
	LCD_WR_DATA8(0x1b);
	LCD_WR_DATA8(0x19);
	LCD_WR_DATA8(0x20);
	LCD_WR_DATA8(0x22);
	LCD_WR_REG(0x29);		
	
#if 0	
	/************************************* Driver IC:ILI9341 ******************************************/	
	//************* Start Initial Sequence **********// 
	LCD_WR_REG(0xCF);  
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0xD9); 
	LCD_WR_DATA8(0X30); 
	 
	LCD_WR_REG(0xED);  
	LCD_WR_DATA8(0x64); 
	LCD_WR_DATA8(0x03); 
	LCD_WR_DATA8(0X12); 
	LCD_WR_DATA8(0X81); 
	 
	LCD_WR_REG(0xE8);  
	LCD_WR_DATA8(0x85); 
	LCD_WR_DATA8(0x10); 
	LCD_WR_DATA8(0x78); 
	 
	LCD_WR_REG(0xCB);  
	LCD_WR_DATA8(0x39); 
	LCD_WR_DATA8(0x2C); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x34); 
	LCD_WR_DATA8(0x02); 
	 
	LCD_WR_REG(0xF7);  
	LCD_WR_DATA8(0x20); 
	 
	LCD_WR_REG(0xEA);  
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x00); 
	 
	LCD_WR_REG(0xC0);    //Power control 
	LCD_WR_DATA8(0x21);   //VRH[5:0] 
	 
	LCD_WR_REG(0xC1);    //Power control 
	LCD_WR_DATA8(0x12);   //SAP[2:0];BT[3:0] 
	 
	LCD_WR_REG(0xC5);    //VCM control 
	LCD_WR_DATA8(0x32); 
	LCD_WR_DATA8(0x3C); 
	 
	LCD_WR_REG(0xC7);    //VCM control2 
	LCD_WR_DATA8(0XC1); 
	 
	LCD_WR_REG(0x36);    // Memory Access Control 
	LCD_WR_DATA8(0xA8);	

	LCD_WR_REG(0x3A);   
	LCD_WR_DATA8(0x55); 

	LCD_WR_REG(0xB1);   
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x18); 
	 
	LCD_WR_REG(0xB6);    // Display Function Control 
	LCD_WR_DATA8(0x0A); 
	LCD_WR_DATA8(0xA2); 
	
	LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
	LCD_WR_DATA8(0x00); 
	 
	LCD_WR_REG(0x26);    //Gamma curve selected 
	LCD_WR_DATA8(0x01); 
	 
	LCD_WR_REG(0xE0);    //Set Gamma 
	LCD_WR_DATA8(0x0F); 
	LCD_WR_DATA8(0x20); 
	LCD_WR_DATA8(0x1E); 
	LCD_WR_DATA8(0x09); 
	LCD_WR_DATA8(0x12); 
	LCD_WR_DATA8(0x0B); 
	LCD_WR_DATA8(0x50); 
	LCD_WR_DATA8(0XBA); 
	LCD_WR_DATA8(0x44); 
	LCD_WR_DATA8(0x09); 
	LCD_WR_DATA8(0x14); 
	LCD_WR_DATA8(0x05); 
	LCD_WR_DATA8(0x23); 
	LCD_WR_DATA8(0x21); 
	LCD_WR_DATA8(0x00); 
	 
	LCD_WR_REG(0xE1);    //Set Gamma 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x19); 
	LCD_WR_DATA8(0x19); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x12); 
	LCD_WR_DATA8(0x07); 
	LCD_WR_DATA8(0x2D); 
	LCD_WR_DATA8(0x28); 
	LCD_WR_DATA8(0x3F); 
	LCD_WR_DATA8(0x02); 
	LCD_WR_DATA8(0x0A); 
	LCD_WR_DATA8(0x08); 
	LCD_WR_DATA8(0x25); 
	LCD_WR_DATA8(0x2D); 
	LCD_WR_DATA8(0x0F); 
	LCD_WR_REG(0x29);    //Display on 
#endif	
} 
