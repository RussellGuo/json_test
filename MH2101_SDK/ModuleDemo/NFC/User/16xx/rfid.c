/**
 ****************************************************************
 * @file rfid.c
 *
 * @brief 
 *
 * @author 
 *
 * 
 ****************************************************************
 */
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "uart.h"
#include "timer.h"
#include "mh523.h"
#include "iso14443a.h" 
#include "iso14443b.h" 
#include "mifare.h" 
#include "iso14443_4.h"
#include "rfid.h"

tag_info g_tag_info;

statistics_t g_statistics;
bool g_statistic_refreshed;

u32 g_cos_loop_times = 0;
u8 g_cos_loop = FALSE;
u8 g_loop_buf[60];//不支持太大的cos指令测试

extern void make_packet(u8 cmd, u8 *buf, u8 len);
void com_halt_a(void);
void com_mf1_read(u8 *pcmd);
void com_mf1_write(u8 *pcmd);
void com_mf0_read(u8 *pcmd);
void com_mf0_write(u8 *pcmd);
void com_halt_b(u8 *pcmd);
void com_sr176_read(u8 *pcmd);
void com_sr176_write(u8 *pcmd);
void com_select_sr(void);
void com_protect_sr176(u8 *pcmd);
void com_complect_sr(void);
void com_get_idcard_num(void);
void auto_write_read_after_reqa(void);
void com_exchange(u8 *pcmd);
void com_multi_exchange_test(u8 *pcmd);
void com_typea_rats(u8 *pcmd);

void rfid_init(void)
{
	memset(&g_tag_info, 0, sizeof(g_tag_info));
	memset(&g_statistics, 0, sizeof(g_statistics));
}
void rfid_operation(u8 *pcmd)
{
	switch(pcmd[0])
	{
		case COM_PKT_CMD_INIT_TYPEA:
			pcd_config('A');							break;
		case COM_PKT_CMD_INIT_TYPEB:
			pcd_config('B');							break;
		//Type A
		case COM_PKT_CMD_REQA:
			pcd_set_rate('1');
			com_reqa(pcmd);								break;		
		case COM_PKT_CMD_TYPEA_HALT:
			com_halt_a();								break;
		case COM_PKT_CMD_TYPEA_MF1_READ:
			com_mf1_read(pcmd);							break;
		case COM_PKT_CMD_TYPEA_MF1_WRITE:
			com_mf1_write(pcmd);						break;
		case COM_PKT_CMD_TYPEA_MF1_WALLET_INIT:
			break;
		case COM_PKT_CMD_TYPEA_MF1_WALLET_READ:
			break;
		case COM_PKT_CMD_TYPEA_MF1_WALLET_INCREMENT:
			break;
		case COM_PKT_CMD_TYPEA_MF1_WALLET_DECREMENT:
			break;
		case COM_PKT_CMD_TYPEA_MF0_READ:
			com_mf0_read(pcmd);
			break;
		case COM_PKT_CMD_TYPEA_MF0_WRITE:
			com_mf0_write(pcmd);
			break;
		case COM_PKT_CMD_TYPEA_RATS:
			com_typea_rats(pcmd);
			break;
		//Type B
		case COM_PKT_CMD_REQB:
			pcd_set_rate('1');
			com_reqb(pcmd);	
			break;
		case COM_PKT_CMD_TYPEB_HALT:
			com_halt_b(pcmd);							break;
		case COM_PKT_CMD_TYPEB_UID:
			com_get_idcard_num();
			break;
		case COM_PKT_CMD_EXCHANGE:
			com_exchange(pcmd);
			break;
		case COM_PKT_CMD_MULTI_EXCHANGE_TEST:
			com_multi_exchange_test(pcmd);
			break;	
		case COM_PKT_CMD_DESELECT:
			iso14443_4_deselect(0);
			break;
		case COM_PKT_CMD_STATISTICS:
			memset(&g_statistics, 0, sizeof(g_statistics));
			break;
		default:
			NFC_PRINT("cmd err\n");
			break;
	}

}
void pcd_sleep(void)
{
	write_reg(CommandReg, 0x10);
}
void pcd_wake(void)
{
	write_reg(CommandReg, 0x20);
}

int com_reqa(u8 *pcmd)
{
	int status;
	u8 sak;
	u8 buf[10];
	
	g_statistics.reqa_cnt++;
	g_statistic_refreshed=TRUE;

	pcd_default_info();


	status = pcd_request(pcmd[1], g_tag_info.tag_type_bytes);
	//一次防冲突及选卡
	if (status == MI_OK)
	{
		g_tag_info.uid_length = UID_4;
		make_packet(COM_PKT_CMD_CARD_TYPE, g_tag_info.tag_type_bytes, sizeof(g_tag_info.tag_type_bytes));
		
		status = pcd_cascaded_anticoll(PICC_ANTICOLL1, 0, &g_tag_info.serial_num[0]);
		if (status == MI_OK)
		{
			status = pcd_cascaded_select(PICC_ANTICOLL1, &g_tag_info.serial_num[0], &sak);
		}
	}
	//二次防冲突及选卡
	if(status == MI_OK && (sak & BIT2))
	{
		g_tag_info.uid_length = UID_7;
		status = pcd_cascaded_anticoll(PICC_ANTICOLL2, 0, &g_tag_info.serial_num[4]);
		if(status == MI_OK)
		{
			status = pcd_cascaded_select(PICC_ANTICOLL2, &g_tag_info.serial_num[4], &sak);
		}
	}
	//三次防冲突及选卡
	if(status == MI_OK && (sak & BIT2))
	{
		g_tag_info.uid_length = UID_10;
		status = pcd_cascaded_anticoll(PICC_ANTICOLL3, 0, &g_tag_info.serial_num[7]);
		if(status == MI_OK)
		{
			status = pcd_cascaded_select(PICC_ANTICOLL3, &g_tag_info.serial_num[7], &sak);
		}
	}
	//回复uid
	if (status == MI_OK)
	{
		buf[0] = g_tag_info.uid_length;
		memcpy(buf+1, (g_tag_info.uid_length == UID_4 ? &g_tag_info.serial_num[0]:&g_tag_info.serial_num[1]), g_tag_info.uid_length);
		make_packet(COM_PKT_CMD_REQA, buf, g_tag_info.uid_length + 1);
	}

	if(status == MI_OK)
	{
	}
	else
	{
		g_statistics.reqa_fail++;
		NFC_PRINT("reqa_fail\n");
		
	}
	
	return status;
}

void com_halt_a(void)
{
	u8 status;
	
	status = pcd_hlta();
	make_packet(COM_PKT_CMD_TYPEA_HALT, &status, sizeof(status));
}

void com_mf1_read(u8 *pcmd)
{	
	u8 status;
	u8 block;
	u8 is_a;
	u8 buf[17];
	
	block = pcmd[1];
	is_a = pcmd[2];
	
	if(g_tag_info.uid_length == UID_4)
	{
		status = pcd_auth_state((is_a == TRUE ? PICC_AUTHENT1A : PICC_AUTHENT1B), block, &g_tag_info.serial_num[0], &pcmd[3]);		
	}
	else if(g_tag_info.uid_length == UID_7)
	{
		status = pcd_auth_state((is_a == TRUE ? PICC_AUTHENT1A : PICC_AUTHENT1B), block, &g_tag_info.serial_num[4], &pcmd[3]);
	}
	else if(g_tag_info.uid_length == UID_10)
	{
		status = pcd_auth_state((is_a == TRUE ? PICC_AUTHENT1A : PICC_AUTHENT1B), block, &g_tag_info.serial_num[7], &pcmd[3]);	
	}

	
	if (status == MI_OK)
	{
		status = pcd_read(block, buf + 1);
	}
	if (status == MI_OK)
	{
		buf[0] = block;
		make_packet(COM_PKT_CMD_TYPEA_MF1_READ, buf, sizeof(buf));
	}
}

void com_mf1_write(u8 *pcmd)
{
	u8 status;
	u8 block;
	u8 is_a;
	
	block = pcmd[1];
	is_a = pcmd[2];

	if(g_tag_info.uid_length == UID_4)
	{
		status = pcd_auth_state((is_a == TRUE ? PICC_AUTHENT1A : PICC_AUTHENT1B), block, &g_tag_info.serial_num[0], &pcmd[3]);		
	}
	else if(g_tag_info.uid_length == UID_7)
	{
		status = pcd_auth_state((is_a == TRUE ? PICC_AUTHENT1A : PICC_AUTHENT1B), block, &g_tag_info.serial_num[4], &pcmd[3]);
	}
	else if(g_tag_info.uid_length == UID_10)
	{
		status = pcd_auth_state((is_a == TRUE ? PICC_AUTHENT1A : PICC_AUTHENT1B), block, &g_tag_info.serial_num[7], &pcmd[3]);	
	}
	
	if (status == MI_OK)
	{
		status = pcd_write(block, &pcmd[9]);
	}
	make_packet(COM_PKT_CMD_TYPEA_MF1_WRITE, &status, sizeof(status));
}

void com_mf0_read(u8 *pcmd)
{
	u8 status;
	u8 block;
	u8 buf[17];
	
	block = pcmd[1];
	
	status = pcd_read(block, buf + 1);

	if (status == MI_OK)
	{
		buf[0] = block;
		make_packet(COM_PKT_CMD_TYPEA_MF0_READ, buf, sizeof(buf));
	}
}

void com_mf0_write(u8 *pcmd)
{
	u8 status;
	u8 block;
	
	block = pcmd[1];

	status = pcd_write_ultralight(block, &pcmd[2]);
	
	make_packet(COM_PKT_CMD_TYPEA_MF0_WRITE, &status, sizeof(status));
}

int com_reqb(u8 *pcmd)
{
	int status;
	u8 i;
	u8 cnt;
	u8 ATQB[16];
	u8 req_code;

	g_statistics.reqb_cnt++;
	g_statistic_refreshed=TRUE;

	req_code = pcmd[1];
	
	cnt = 3;//应用中 可以使用轮询N次	
	while(cnt--)
	{
		status = pcd_request_b(req_code, 0, 0, ATQB);
		
		if(status == MI_COLLERR)	// 有冲突，超过一张卡
		{
			if((status = pcd_request_b(req_code, 0, 2, ATQB)) != MI_OK)
			{
			   	for (i = 1; i < 4; i++)
			   	{
			    	if((status = pcd_slot_marker(i, ATQB)) == MI_OK) 
					{
						break;
			    	}
			  	}
				if (status == MI_OK)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		else if (status == MI_OK)
		{
			break;
		}
	}

	if (status == MI_OK)
	{
		//typeB 106默认速率
		status = pcd_attri_b(&ATQB[1], 0, ATQB[10]&0x0f, PICC_CID, ATQB);

		if (status == MI_OK)
		{						
			ATQB[0] = 0x50;//恢复默认值
			make_packet(COM_PKT_CMD_REQB, ATQB, 12);
		}
	}
	
	if(status == MI_OK)
	{
	}
	else
	{		
		g_statistics.reqb_fail++;
		NFC_PRINT("reqb_fail\n");

	}
	
	return status;
}

void com_halt_b(u8 *pcmd)
{
	u8 status;
	pcmd = pcmd;

	status = iso14443_4_deselect(PICC_CID); //deselect all card
	//status = pcd_halt_b(pcmd + 1);// 4 bytes uid
	
	make_packet(COM_PKT_CMD_TYPEB_HALT, &status, sizeof(status));
}
void com_get_idcard_num(void)
{
	u8 id[10];
	int status;
	
	status = get_idcard_num(id);
	if (status == MI_OK)
	{
		make_packet(COM_PKT_CMD_TYPEB_UID, id, 10);
	}
}

void statistic_print()
{
	statistics_t statistics_tmp;
	
	if (g_statistic_refreshed == TRUE)
	{
		mdelay(20);
		memcpy(&statistics_tmp, &g_statistics, sizeof(g_statistics));
		
		statistics_tmp.reqa_cnt = htonl(statistics_tmp.reqa_cnt);
		statistics_tmp.reqa_fail = htonl(statistics_tmp.reqa_fail);
		statistics_tmp.reqb_cnt = htonl(statistics_tmp.reqb_cnt);
		statistics_tmp.reqb_fail = htonl(statistics_tmp.reqb_fail);
		statistics_tmp.write_fail = htonl(statistics_tmp.write_fail);
		statistics_tmp.read_fail = htonl(statistics_tmp.read_fail);
		statistics_tmp.check_fail = htonl(statistics_tmp.check_fail);
		statistics_tmp.atqa_fail = htonl(statistics_tmp.atqa_fail);
		statistics_tmp.cos_fail = htonl(statistics_tmp.cos_fail);
		statistics_tmp.atqa_cnt = htonl(statistics_tmp.atqa_cnt);
		statistics_tmp.cos_cnt = htonl(statistics_tmp.cos_cnt);
		statistics_tmp.lpcd_cnt = htonl(statistics_tmp.lpcd_cnt);
		statistics_tmp.lpcd_fail = htonl(statistics_tmp.lpcd_fail);
		
		make_packet(COM_PKT_CMD_STATISTICS, (u8*)&statistics_tmp, sizeof(statistics_tmp));
		g_statistic_refreshed = FALSE;
	}
}
void com_typea_rats(u8 * pcmd)
{
	int status;
	u8 CID = 0;
	u8 ats[3];
	u8 rate;   
	transceive_buffer *pi;
	

	pi = &mf_com_data;
	
	status = pcd_rats_a(CID, ats);
	if(status == MI_OK)
	{		
		if (pcmd[1] != 0)
		{//需要做pps指令
			rate = pcmd[1];
			status = pcd_pps_rate(pi, ats, CID, rate);
		}
	}

	if (status != MI_OK)
	{
		g_statistics.atqa_fail++;
	}
	g_statistic_refreshed = TRUE;
	
}
void com_exchange(u8 * pcmd)
{			
	u8 loop_buf[256];
	int status;
	
	u16 tx_len;
	u32 rx_len;
	
	g_statistics.cos_cnt++;

	tx_len = pcmd[1];
	memcpy(loop_buf, pcmd + 2, tx_len);
	status = ISO14443_4_HalfDuplexExchange(&g_pcd_module_info, loop_buf, tx_len, loop_buf, &rx_len);
	if (status == MI_OK && rx_len < sizeof(loop_buf))
	{
		memmove(loop_buf+1, loop_buf, rx_len);
		loop_buf[0]=rx_len;
		make_packet(COM_PKT_CMD_EXCHANGE, (u8*)&loop_buf, rx_len + 1);		
	}
	if (status != MI_OK)
	{
		g_statistics.cos_fail++;
	}
	g_statistic_refreshed = TRUE;
}



void com_multi_exchange_test(u8 *pcmd)
{
	g_cos_loop = TRUE;
	if(pcmd[1 + sizeof(u32)] + 1 <= sizeof(g_loop_buf))
	{
		g_cos_loop_times = *(u32*)&pcmd[1];
		g_loop_buf[0]= pcmd[sizeof(u32)];
		memcpy(g_loop_buf + 1, pcmd + 1 + sizeof(u32), pcmd[1 + sizeof(u32)] + 1);
	}
	else
	{
		printf("multi cos data > g_loop_buf\n");
	}
}

