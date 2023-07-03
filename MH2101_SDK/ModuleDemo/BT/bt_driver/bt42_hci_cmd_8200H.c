#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "bt42_uart.h"
#include "bt42_timer.h"
#include "bt42_patch_8200H.h"
#include "bt42_hci_cmd.h"
#include "mhscpu_gpio.h"


bluetooth_param bluetooth_param_t ={
	.cmdrespFlag = 0,
};

static uint32_t rx_len = 0;
void (*event_cb)(uint8_t type,uint16_t len,uint8_t *pbuf) = NULL;


void bt42_event_cb_register(void (*api_event_cb)(uint8_t type,uint16_t len,uint8_t *pbuf))
{
	event_cb = api_event_cb;
}

void bt42_event_process(void)
{
	uint16_t uilen;
	uilen = host_uart_read(bluetooth_param_t.eventbuf + rx_len,3 - rx_len);
	rx_len += uilen;
	if((rx_len > 0) && (BT42_HCI_EVENT_TYPE != bluetooth_param_t.eventbuf[0])) rx_len = 0;
	if(rx_len == 3)
	{
		uint8_t eventype = bluetooth_param_t.eventbuf[1];
		uint16_t eventlen = bluetooth_param_t.eventbuf[2];
		uint16_t read_len;
		uint16_t datalen = bluetooth_param_t.eventbuf[2];
		if(eventype == HCI_EVENT_SPP_ENHANCEMENT_DATA_RECEIVED) 
		{
				host_uart_read(bluetooth_param_t.eventbuf + rx_len,1);
				rx_len += 1;
				eventlen = datalen = ((uint16_t)bluetooth_param_t.eventbuf[3] << 8) + bluetooth_param_t.eventbuf[2];
		}
		while(1)
		{
			if(eventype == 0xFC) break;
			read_len = host_uart_read(bluetooth_param_t.eventbuf + rx_len,datalen);
			if(read_len < datalen)
			{
				rx_len += read_len;
				datalen -= read_len;
			}
			else
			{
				break;
			}
		}
		rx_len = 0;
		if(event_cb != NULL){
			if(eventype == HCI_EVENT_SPP_ENHANCEMENT_DATA_RECEIVED){
				event_cb(eventype,eventlen,&bluetooth_param_t.eventbuf[4]);
			}else{
				event_cb(eventype,eventlen,&bluetooth_param_t.eventbuf[3]);
			}
		}
	}
}

uint8_t bt42_hci_sendcmd(uint8_t opcode,uint8_t *data,uint8_t datalen)
{
	CmdFIFO_t  Bt_cmd;
	uint8_t bRet;
	Bt_cmd.len = datalen + 3;
	Bt_cmd.data[0]  = BT42_HCI_CMD_TYPE;
	Bt_cmd.data[1] = opcode;
	Bt_cmd.data[2] = datalen;
	if(data != NULL) memcpy(&Bt_cmd.data[3],data,datalen);
	host_uart_send((uint8_t *)Bt_cmd.data,Bt_cmd.len);
	bRet = BT_SUCCESS;
	
	return bRet;
}

uint8_t bt42_hci_recvevent(uint8_t opcode)
{
	uint8_t ret = BT_ERROR;
	uint32_t now_tick = get_tick_count();
	uint32_t utick = 0;
	while(false == bluetooth_param_t.cmdrespFlag && utick < 5000)
	{
		bt42_event_process();
		utick = get_tick_count() - now_tick;
	}
	if(true == bluetooth_param_t.cmdrespFlag)
	{
		if(opcode == HCI_EVENT_STATUS_RESPONSE)
		{
			if(1 == bluetooth_param_t.cmdresplen)
			{
				ret = BT_SUCCESS;
			}
		}
		else
		{
			if ((bluetooth_param_t.cmdresplen >= 2)
                    && (bluetooth_param_t.cmdrspdata[0] == opcode)
                    && (bluetooth_param_t.cmdrspdata[1] == 0))
			{
				 
				 ret = BT_SUCCESS;
			}
		}
	}
	bluetooth_param_t.cmdrespFlag = false;
	return ret;
}

uint8_t BT_SetBtName(uint8_t *pbuff,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SET_BT_NAME,pbuff,len)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SET_BT_NAME)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetBleName(uint8_t *pbuff,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SET_BLE_NAME,pbuff,len)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SET_BLE_NAME)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetBtMacAddr(uint8_t *pbuff,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SET_BT_ADDR,pbuff,len)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SET_BT_ADDR)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetBleMacAddr(uint8_t *pbuff,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SET_BLE_ADDR,pbuff,len)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SET_BLE_ADDR)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetVisibility(uint8_t mode)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SET_VISIBILITY,&mode,sizeof(mode))
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SET_VISIBILITY)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetPairMode(uint8_t mode)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SET_PAIRING_MODE,&mode,sizeof(mode))
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SET_PAIRING_MODE)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetPinCode(uint8_t *pbuf,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SET_PINCODE,pbuf,len)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SET_PINCODE)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetBLEPairMode(uint8_t mode)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(0x33,&mode,1)
		&& (BT_SUCCESS == bt42_hci_recvevent(0x33)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_BLEGETKEY(uint8_t mode)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(0x48,&mode,1)
		&& (BT_SUCCESS == bt42_hci_recvevent(0x48)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetUartFlow(uint8_t state)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SET_UART_FLOW,&state,1)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SET_UART_FLOW)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetUartBaud(uint32_t baud)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SET_UART_BAUD,&baud,sizeof(baud))
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SET_UART_BAUD)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_GetVersion(void)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_VERSION_REQUEST,NULL,0)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_VERSION_REQUEST)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SPPDisconect(void)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_BT_DISCONNECT,NULL,0)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_BT_DISCONNECT)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_BLEDisconect(void)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_BLE_DISCONNECT,NULL,0)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_BLE_DISCONNECT)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_UpdataConnParam(uint8_t *pbuff,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_UPDATE_CONN_PARAM,pbuff,len)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_UPDATE_CONN_PARAM)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetCod(uint8_t *pbuf,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SET_COD,pbuf,len)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SET_COD)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetNVRAM(uint8_t *pbuf,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SET_NVRAM,pbuf,len)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SET_NVRAM)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_EnterSleep(uint8_t state)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_ENTER_SLEEP_MODE,&state,1)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_ENTER_SLEEP_MODE)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_GetVolatge(uint8_t pin)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_POWER_REQ,&pin,1)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_POWER_REQ)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetAdvData(uint8_t *pbuf,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SET_ADV_DATA,pbuf,len)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SET_ADV_DATA)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetScanRespData(uint8_t *pbuf,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SET_SCAN_RESP_DATA,pbuf,len)
		&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SET_SCAN_RESP_DATA)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_WriteReg(uint8_t *pbuf,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(0x81,pbuf,len)
		&& (BT_SUCCESS == bt42_hci_recvevent(0x81)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_ReadReg(uint8_t *pbuf,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(0x80,pbuf,len)
		&& (BT_SUCCESS == bt42_hci_recvevent(0x80)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

uint8_t BT_SetBLELen(uint8_t *pbuf,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	if(BT_SUCCESS == bt42_hci_sendcmd(0x37,pbuf,len)
		&& (BT_SUCCESS == bt42_hci_recvevent(0x37)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

int BT_PatchCmd(uint8_t *pbuff,uint8_t len)
{
	uint8_t ret = BT_ERROR;
	uint8_t opcode = pbuff[1];
	uint8_t uilen = len - 3;
	if((BT_SUCCESS == bt42_hci_sendcmd(opcode,&pbuff[3],uilen))
		&& (BT_SUCCESS == bt42_hci_recvevent(opcode)))
	{
		ret = BT_SUCCESS;
	}
	return ret;
}

void BT_Patch_Download(void)
{
	int index = 0;
	int cmd_len;
	uint8_t cmd_buf[300];
	while(1)
	{
		cmd_len = bt42_patch[index];
		index++;
		if (index >= sizeof(bt42_patch))
		{
			bt42_event_process();
			if(bluetooth_param_t.bBtReadyFlag) return;
			continue;
		}
		memcpy(cmd_buf,&bt42_patch[index],cmd_len);
		if(BT_SUCCESS != BT_PatchCmd(cmd_buf,cmd_len)) return;
		index += cmd_len;
	}
}

void BT_SppSendData(uint8_t *data,uint32_t len)
{
	uint32_t datalen = len;
	uint8_t uilen = 0;
	uint32_t offset=0;
	if((data != NULL) && (len > 0))
	{
		while(datalen > 0)
		{
			if(datalen >= MAX_SPP_MTU)
			{
				uilen = MAX_SPP_MTU;
			}
			else
			{
				uilen = datalen;
			}
			if((BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SEND_SPP_DATA,data+offset,uilen))
				&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SEND_SPP_DATA)))
			{
				offset += uilen;
				datalen = datalen - uilen;
			}
			
			
		}
	}
}

void BT_BleSendData(uint16_t handle,uint8_t *data,uint32_t len)
{
	uint32_t datalen = len;
	uint8_t uilen = 0;
	uint32_t offset=0;
	uint8_t sendbuf[300];
	sendbuf[0] = handle & 0xFF;
	sendbuf[1] = (handle >> 8) & 0xFF;
	if((data != NULL) && (len > 0))
	{
		while(datalen > 0)
		{
			if(datalen >= MAX_BLE_MTU)
			{
				uilen = MAX_BLE_MTU;
			}
			else
			{
				uilen = datalen;
			}
			memcpy(sendbuf + 2,data+offset,uilen);
			if((BT_SUCCESS == bt42_hci_sendcmd(HCI_CMD_SEND_BLE_DATA,sendbuf,uilen + 2))
				&& (BT_SUCCESS == bt42_hci_recvevent(HCI_CMD_SEND_BLE_DATA)))
			{
				offset += uilen;
				datalen = datalen - uilen;
			}
		}
	}
}

