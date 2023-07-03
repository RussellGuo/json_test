 /**
 ****************************************************************
 * @file define.h
 *
 * @brief  define����ƺͲ��ֱ�������
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


#define NFC_DEBUG		    1	// printf��ӡ���ƿ���

#if NFC_DEBUG 
#define NFC_PRINT(format,...) printf(format,##__VA_ARGS__)
#else
#define NFC_PRINT(format,...) {}
#endif


#define POWERON_POLLING     1   //�ϵ缴��ʼ�Զ�A/B polling
#define INT_USE_CHECK_REG   1	//�жϼ��ʹ�ò�ѯ07�Ĵ�����irq bit�ķ�ʽ��������ʹ�ò�ѯ�жϹܽ�

#define	NFC_INT_GPIOX	    GPIOE
#define	NFC_INT_GPIO_PIN	GPIO_Pin_10

/*********************ϵͳʱ������************************/



/************************PC���������λ�����ͨ��������**************************/
#define COM_PKT_CMD_READ_REG 		      1
#define COM_PKT_CMD_WRITE_REG	          2

#define COM_PKT_CMD_QUERY_MODE		      0x0D	//�Ƿ����ֶ�ģʽ���������Զ�Ѱ��ģʽ
#define COM_PKT_CMD_CHIP_RESET            0x0E  //��λ����оƬ
#define COM_PKT_CMD_CARD_TYPE		      0x0F
#define COM_PKT_CMD_REQ_SELECT		      0x10
#define COM_PKT_CMD_HALT			      0x11  //����Haltָ��
#define COM_PKT_CMD_STATISTICS		      0x12  //ͳ����Ϣ
#define COM_PKT_CMD_LPCD			      0x13  //LPCD ����
#define COM_PKT_CMD_LPCD_CONFIG_TEST      0x16  //����LPCD����
#define COM_PKT_CMD_LPCD_CONFIG_TEST_STOP 0x17


typedef unsigned int  u32;
typedef unsigned short u16;
typedef unsigned char  u8;
typedef unsigned char  bool;


#endif
