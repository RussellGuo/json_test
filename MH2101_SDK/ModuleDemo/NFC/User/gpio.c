/* Include ------------------------------------------------------------------*/
#include "mhscpu.h"
#include "define.h"
#include "gpio.h"
#include "spi.h"
#include "mh523.h"

/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/	

/* Private macro ------------------------------------------------------------*/

#define	NFC_PD_GPIOX	  GPIOE
#define	NFC_PD_GPIO_PIN	  GPIO_Pin_11

/* Ptivate function prototypes ----------------------------------------------*/	

/******************************************************************************
* Function Name  : gpio_config
* Description    : config gpio for led and nrstpd
* Input          : NONE
* Output         : NONE
* Return         : NONE
******************************************************************************/
void gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    
    /// NRSTPD
	GPIO_InitStruct.GPIO_Pin = NFC_PD_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(NFC_PD_GPIOX, &GPIO_InitStruct);

    /// NFC INT
	GPIO_InitStruct.GPIO_Pin = NFC_INT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(NFC_INT_GPIOX, &GPIO_InitStruct);
    
    /// NFC CSN
	GPIO_InitStruct.GPIO_Pin = NFC_SPI_CS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(NFC_SPI_CS_GPIOx, &GPIO_InitStruct);     
}

/******************************************************************************
* Function Name  : led_success_on
* Description    : len on
* Input          : NONE
* Output         : NONE
* Return         : NONE
******************************************************************************/
void led_success_on(void)
{
    write_reg(TestPinEnReg, 0x9E);
    write_reg(TestPinValueReg, 0x80);	 
}

/******************************************************************************
* Function Name  : led_success_off
* Description    : len off
* Input          : NONE
* Output         : NONE
* Return         : NONE
******************************************************************************/
void led_success_off(void)
{
    write_reg(TestPinEnReg, 0x9E);
    write_reg(TestPinValueReg, 0x9E);		   
}

/******************************************************************************
* Function Name  : pcd_poweron
* Description    : pcd power on
* Input          : NONE
* Output         : NONE
* Return         : NONE
******************************************************************************/
void pcd_poweron(void)
{
	int i;
	
	GPIO_SetBits(NFC_PD_GPIOX, NFC_PD_GPIO_PIN); 
	for (i = 0; i < 2000; i++);
}

/******************************************************************************
* Function Name  : pcd_powerdown
* Description    : pcd power down
* Input          : NONE
* Output         : NONE
* Return         : NONE
******************************************************************************/
void pcd_powerdown(void)
{
   GPIO_ResetBits(NFC_PD_GPIOX, NFC_PD_GPIO_PIN); 
}


/********************** (C) COPYRIGHT 2014 Megahuntmicro ********************/
