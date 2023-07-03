#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "bt42_uart.h"
#include "bt42_timer.h"
#include "hw_config.h"
#include "bt42_hci_cmd.h"
#include "multi_timer.h"


#define MAX_RECEIVE_BUFFER_SIZE			1024
	
#define BLE_TX_HANDLE						3	

static uint8_t bt_name[]="820H-SPP";
static uint8_t ble_name[]="820H-BLE";

static uint8_t bt_addr[6]={0x32, 0x21, 0xD7, 0x26, 0x72, 0x0c};//br地址
static uint8_t ble_addr[6]={0x11, 0x11, 0x16, 0x09, 0x23, 0x00};//ble地址 

uint8_t AdvData[] = {
	0x02,
	0x01,
	0x06,
	0x09,
	0x09,
	'8','2','0','H','-','B','L','E',
	0x03,
	0x03,
	0xE7,0xFE,
};

uint8_t receive_buf[MAX_RECEIVE_BUFFER_SIZE];
uint32_t receive_len = 0;

void bt42_api_event(uint8_t type,uint16_t len,uint8_t *pbuf);
void bluetooth_app_init(void);

void bt42_bluetooth_init(void)
{
	bt42_event_cb_register(bt42_api_event);
	bt_pin_init();
	bt_uart_configuration();
	bt_timer_configuration();
	bt_power_off();
	m_delay(20);
	bt_power_on();
}


/*
初始化配置相关：
1.名字和地址
2.MCU下发NVRAM数据
3.可发现模式
4.包括uart流控和波特率及使能休眠指令等操作相关的指令都放入该接口中即可
*/
void bluetooth_app_init(void)
{
	m_delay(100);//patch后下延迟至少50ms再发配置指令
	BT_SetBtName(bt_name,sizeof(bt_name));
	BT_SetAdvData(AdvData,sizeof(AdvData));
	BT_SetBtMacAddr(bt_addr,sizeof(bt_addr));
	BT_SetBleMacAddr(ble_addr,sizeof(ble_addr));
	BT_SetVisibility(0x07);

}

/*
事件处理函数：
1.连接和断开的处理
2.手机下发的数据处理
*/
void bt42_api_event(uint8_t type,uint16_t len,uint8_t *pbuf)
{
	switch(type)
	{
		case HCI_EVENT_I_AM_READY:
		{
			bluetooth_param_t.bBtReadyFlag = 1;
			printf("ready...\n");
		}break;
		case HCI_EVENT_BT_CONNECTED:
		{
			bluetooth_param_t.uConnectTpye = BT_STATUS_SSP_CONNECTED;
			printf("bt connected...\n");
			
		}break;
		case HCI_EVENT_BLE_CONNECTED:
		{
			bluetooth_param_t.uConnectTpye = BT_STATUS_BLE_CONNECTED;
			printf("ble connected...\n");
		}break;
		
		case HCI_EVENT_BLE_DISCONNECTED:
		case HCI_EVENT_BT_DISCONNECTED:
		{
			bluetooth_param_t.uConnectTpye = BT_STATUS_NO_CONNECTED;
			printf("bt disconected...\n");
		}break;
		
		case HCI_EVENT_SPP_ENHANCEMENT_DATA_RECEIVED:
		case HCI_EVENT_SPP_DATA_RECEIVED:
		memcpy(receive_buf + receive_len,pbuf,len);
		receive_len += len;
		if(receive_len >= MAX_RECEIVE_BUFFER_SIZE){
			receive_len = 0;
			BT_SppSendData(receive_buf,MAX_RECEIVE_BUFFER_SIZE);
		}
		break;
					
		case HCI_EVENT_BLE_DATA_RECEIVED:
		/*
			数据内容是从pbuf[2]开始，前面两个字节是代表的接收handle，可以不用关注
			数据有效长度是len - 2
		*/
		{
			BT_BleSendData(BLE_TX_HANDLE,&pbuf[2],len - 2);
		}break;	

		case HCI_EVENT_STATUS_RESPONSE:
		case HCI_EVENT_CMD_COMPLETE:
		{
			bluetooth_param_t.cmdresplen = len;
			memcpy(bluetooth_param_t.cmdrspdata, pbuf, bluetooth_param_t.cmdresplen);
			bluetooth_param_t.cmdrespFlag = true;
		}break;
		
		case HCI_EVENT_NVRAM_CHANGED:
		/*1.需要把数据存储到MCU flash中去
		  2.在上电初始化时加载flash内容调用BT_SetNVRAM
		*/
		break;
		
		case HCI_EVENT_POWER_AND_ERROR:
		{
				BT_Patch_Download();
				bluetooth_app_init();
		}break;
		
		default:
			break;
	}
}
