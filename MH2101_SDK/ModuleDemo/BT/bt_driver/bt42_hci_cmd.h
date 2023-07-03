#ifndef __BT42_HCI_CMD_H
#define __BT42_HCI_CMD_H

#include <stdio.h>
#include <stdint.h>


#define EVENT_BUFFER_MAX_SIZE			400
#define SEND_MAX_DATA_SIZE				300

#define MAX_SPP_MTU								120
#define MAX_BLE_MTU								120

#define BT42_HCI_CMD_TYPE					0x01
#define BT42_HCI_EVENT_TYPE				0x02


/*********CMD�������� Opcode ����������***/
#define HCI_CMD_SET_BT_ADDR        0x00     //���� BT3.0 ��ַ
#define HCI_CMD_SET_BLE_ADDR       0x01     //���� BLE ��ַ
#define HCI_CMD_SET_VISIBILITY     0x02     //���ÿɷ��ֺ͹㲥
#define HCI_CMD_SET_BT_NAME        0x03     //���� BT3.0 ����
#define HCI_CMD_SET_BLE_NAME       0x04     //���� BLE ����
#define HCI_CMD_SEND_SPP_DATA      0x05     // ���� BT3.0��SPP������
#define HCI_CMD_SEND_BLE_DATA      0x09 	//���� BLE ����
#define HCI_CMD_SEND_DATA          0x0A 	//�������ݣ��Զ�ѡ��ͨ����
#define HCI_CMD_STATUS_REQUEST     0x0B 	//��������״̬
#define HCI_CMD_SET_PAIRING_MODE   0x0C 	//�������ģʽ
#define HCI_CMD_SET_PINCODE        0x0D 	//���������
#define HCI_CMD_SET_UART_FLOW      0x0E 	//���� UART ����
#define HCI_CMD_SET_UART_BAUD      0x0F 	//���� UART ������
#define HCI_CMD_VERSION_REQUEST    0x10 	//��ѯģ��̼��汾
#define HCI_CMD_BT_DISCONNECT      0x11		//�Ͽ� BT3.0 ����
#define HCI_CMD_BLE_DISCONNECT     0x12 	//�Ͽ� BLE ����
#define HCI_CMD_SET_COD            0x15 	//���� COD
#define HCI_CMD_SET_NVRAM          0x26     //�·� NV ����
#define HCI_CMD_ENTER_SLEEP_MODE   0x27 	//����˯��ģʽ
#define HCI_CMD_CONFIRM_GKEY       0x28 	//Numeric Comparison ��Է�ʽ�ж���Կ�ıȽ�
#define HCI_CMD_SPP_DATA_COMPLETE  0x29 	//SPP ���ݴ������
#define HCI_CMD_SET_ADV_DATA       0x2A 	//���� ADV ����
#define HCI_CMD_SET_SCAN_RESP_DATA	0x2E	//����ɨ����Ӧ����
#define HCI_CMD_POWER_REQ          0x2B 	//��ѯģ���Դ��ѹ
#define HCI_CMD_POWER_SET          0x2C 	//��ȡ��Դ��ѹ���ܿ���
#define HCI_CMD_PASSKEY_ENTRY      0x30 	//���� PASSKEY ��Է�ʽ�ж���Կ�ıȽ�
#define HCI_CMD_UPDATE_CONN_PARAM  0x36		//ģ���������Ӳ�������

/********EVENT�¼����� Opcode ����������**/
#define HCI_EVENT_BT_CONNECTED 	   	0x00 	//BT3.0 ���ӽ���
#define HCI_EVENT_BLE_CONNECTED    	0x02 	//BLE ���ӽ���
#define HCI_EVENT_BT_DISCONNECTED  	0x03 	//BT3.0 ���ӶϿ�
#define HCI_EVENT_BLE_DISCONNECTED 	0x05 	//BLE ���ӶϿ�
#define HCI_EVENT_CMD_COMPLETE     	0x06 	//���������
#define HCI_EVENT_SPP_DATA_RECEIVED 0x07 	//���յ� BT3.0 ���ݣ�SPP��
#define HCI_EVENT_BLE_DATA_RECEIVED 0x08 	//���յ� BLE ����
#define HCI_EVENT_I_AM_READY 				0x09 	//ģ��׼����
#define HCI_EVENT_STATUS_RESPONSE 	0x0A 	//״̬�ظ�
#define HCI_EVENT_NVRAM_CHANGED 		0x0D 	//�ϴ� NVRAM ����
#define HCI_EVENT_UART_EXCEPTION 		0x0F 	//HCI ����ʽ����
#define HCI_EVENT_GKEY 							0x0E 	//���� Numeric Comparison ��Է�ʽ�в�������Կ
#define HCI_EVENT_GET_PASSKEY 			0x10 	//PASSKEY ��Է�ʽ��֪ͨMCU ������Կ
#define HCI_EVENT_PAIRING_COMPLETED 0x11
#define HCI_EVENT_REMOTE_MTU        0x12
#define HCI_EVENT_BLE_CCC						0x30
#define HCI_EVENT_SPP_ENHANCEMENT_DATA_RECEIVED	0x20
#define HCI_EVENT_POWER_AND_ERROR		0xFC

enum{
	BT_SUCCESS = 0x00,
	BT_ERROR
};

typedef struct 
{
	uint8_t len;
	uint8_t data[SEND_MAX_DATA_SIZE];
}CmdFIFO_t;

typedef struct bluetooth_param_tag
{
	 uint8_t bBtReadyFlag;
	 uint8_t uConnectTpye;
	 uint8_t bisConnectFlag;
	 uint8_t cmdrespFlag;
	 uint8_t cmdresplen;
	 uint8_t cmdrspdata[64];
	 uint8_t eventbuf[EVENT_BUFFER_MAX_SIZE];
}bluetooth_param;

enum
{
	BT_STATUS_NO_CONNECTED = 0x00,
	BT_STATUS_SSP_CONNECTED = 0x01,
	BT_STATUS_BLE_CONNECTED = 0x02,
};

extern bluetooth_param bluetooth_param_t;

void bt42_event_process(void);
void bt42_event_cb_register(void (*api_event_cb)(uint8_t type,uint16_t len,uint8_t *pbuf));
uint8_t BT_SetBtName(uint8_t *pbuff,uint8_t len);
uint8_t BT_SetBleName(uint8_t *pbuff,uint8_t len);
uint8_t BT_SetBtMacAddr(uint8_t *pbuff,uint8_t len);
uint8_t BT_SetBleMacAddr(uint8_t *pbuff,uint8_t len);
uint8_t BT_SetVisibility(uint8_t mode);
uint8_t BT_SetPairMode(uint8_t mode);
uint8_t BT_SetPinCode(uint8_t *pbuf,uint8_t len);
uint8_t BT_SetUartFlow(uint8_t state);
uint8_t BT_SetUartBaud(uint32_t baud);
uint8_t BT_GetVersion(void);
uint8_t BT_SPPDisconect(void);
uint8_t BT_BLEDisconect(void);
uint8_t BT_UpdataConnParam(uint8_t *pbuff,uint8_t len);
uint8_t BT_SetCod(uint8_t *pbuf,uint8_t len);
uint8_t BT_SetNVRAM(uint8_t *pbuf,uint8_t len);
uint8_t BT_EnterSleep(uint8_t state);
uint8_t BT_GetVolatge(uint8_t pin);
void BT_Patch_Download(void);
void BT_SppSendData(uint8_t *data,uint32_t len);
void BT_BleSendData(uint16_t handle,uint8_t *data,uint32_t len);
uint8_t BT_SetAdvData(uint8_t *pbuf,uint8_t len);
uint8_t BT_SetScanRespData(uint8_t *pbuf,uint8_t len);
uint8_t BT_WriteReg(uint8_t *pbuf,uint8_t len);
uint8_t BT_ReadReg(uint8_t *pbuf,uint8_t len);
uint8_t BT_SetBLELen(uint8_t *pbuf,uint8_t len);
#endif
