#include "mhscpu.h"
#include "mhscpu_it.h"
#include <string.h>
#include <stdio.h>
#include "mhscpu_msr.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}


int cnts = 0;
void MSR_IRQHandler(void)
{
	track_data tdata[MAX_TRACK_NUM];
	int ret;
	int i, j;
	
	uint8_t cfmt, tflag;
	
	{
		switch (detect_swiping_card())
		{
		case DETECT_SWIPING_CARD:
			{
				printf("\r\nDetect swiping card,Times: %d \n", ++cnts);
				cfmt = tflag = 0;
				ret = get_decode_data(tdata, TRACK_SELECT_1 | TRACK_SELECT_2 | TRACK_SELECT_3, &cfmt, &tflag);
				printf("T1 = %d, T2 = %d, T3 = %d\ttflag = %02X\n", (int)tdata[0].len, (int)tdata[1].len, (int)tdata[2].len, (int)tflag);
				if (ret == SUCCESS)		
				{
					for (i = 0; i < MAX_TRACK_NUM; i++)
					{
						if (tdata[i].len)
						{
							printf("T%d decode data:\n", (int)(i+1));
							for (j = 0; j < tdata[i].len; j++)
							{
								putchar(tdata[i].buf[j]);
							}
							printf("\n");
						}
					}
				}
			}
			break;
	
		case DETECT_HARD_WAKEUP:
			clear_dpu_int();	//ÇåÖÐ¶Ï
			sc_sleep();	
		
			break;
			
		case DETECT_NO_SWIPING_CARD:
		default:
			clear_dpu_int();
			break;
		}
	}
	
}

