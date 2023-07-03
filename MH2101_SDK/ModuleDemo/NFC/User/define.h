 /**
 ****************************************************************
 * @file define.h
 *
 * @brief  define宏控制和部分变量定义
 *
 * @author 
 *
 * 
 ****************************************************************
 */ 
#ifndef DEFINE_H
#define DEFINE_H


/*
 * VESRION DESCRIPTION
 ****************************************************************
 */	
#define VERSION "1.19.0"


#define NFC_DEBUG		    1	// printf打印控制开关

#if NFC_DEBUG 
#define NFC_PRINT(format,...) printf(format,##__VA_ARGS__)
#else
#define NFC_PRINT(format,...) {}
#endif


#define POWERON_POLLING     1   //上电即开始自动A/B polling
#define INT_USE_CHECK_REG   1	//中断检测使用查询07寄存器的irq bit的方式，而不是使用查询中断管脚

#define	NFC_INT_GPIOX	    GPIOE
#define	NFC_INT_GPIO_PIN	GPIO_Pin_10

/*********************系统时钟配置************************/



/************************PC机软件与下位机软件通信命令字**************************/
#define COM_PKT_CMD_READ_REG 		      1
#define COM_PKT_CMD_WRITE_REG	          2

#define COM_PKT_CMD_QUERY_MODE		      0x0D	//是否是手动模式，而不是自动寻卡模式
#define COM_PKT_CMD_CHIP_RESET            0x0E  //软复位数字芯片
#define COM_PKT_CMD_CARD_TYPE		      0x0F
#define COM_PKT_CMD_REQ_SELECT		      0x10
#define COM_PKT_CMD_HALT			      0x11  //发送Halt指令
#define COM_PKT_CMD_STATISTICS		      0x12  //统计信息
#define COM_PKT_CMD_LPCD			      0x13  //LPCD 功能
#define COM_PKT_CMD_LPCD_CONFIG_TEST      0x16  //测试LPCD功能
#define COM_PKT_CMD_LPCD_CONFIG_TEST_STOP 0x17


typedef unsigned int  u32;
typedef unsigned short u16;
typedef unsigned char  u8;
typedef unsigned char  bool;


#endif
