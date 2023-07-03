#include "ProjectConfig.h"
#include "DecodeLib.h"
#include "beep.h"
/**
 *单buff解码Demo
 *
 */
 void SingleBuffDecodeDemo(void)
 {
    //定义保存解码结果的数组
    uint8_t result[2100] = {0};
    int32_t resnum;    //返回解码结果数量，为0，表示解码失败，为-1表示解码数组偏小
    DecodeConfigTypeDef DecodeCfg = {0};  //设置解那种码
    DecodeResultTypeDef res = {.result = result, .maxn = 2100}; //初始化译码结构体
    
    //配置解码信息
    /**
     * 解码默认配置
     *
     * cfg->cfgCODE128 = (1u | (0u << DE_MIN_LEN) | (0xffu << DE_MAX_LEN));
     * cfg->cfgCODE39 = (1u | (0u << DE_MIN_LEN) | (0xffu << DE_MAX_LEN));     * 
     * cfg->cfgQRCode = 1u | 1u << (DEQR_MISSINGCORNER) | (1u << DEQR_CURVE);
     * 
     * cfg->cfgGlobal = cfg->cfgGlobal = (1u << DEBAR_PRECISION);
     * 
     */
    DecodeConfigInit(&DecodeCfg);

	while(1) 
	{
        //使能采图
        DecodeDcmiStart();
        //检测采图是否完成，需要中断调用DCMI_CallBackFrame()函数
        while( !DecodeDcmiFinish());
        
        //开始解码
        resnum = DecodeStart(&DecodeCfg, &res);
        
        if(resnum > 0)
        {
            printf("ID:%d\tAIMID:%s\n", res.id, res.AIM);
            for(int i = 0; i < resnum; i++)
            {
                printf("%c", result[i]);
            }
            printf("\n");

            beep(200);       
        }
        else if(resnum < 0)
        {
            if(resnum == DecodeResOverflowError)
            {
                printf("解码结果溢出\n");
            }
            else if(resnum == DecodeImageNoDoneError)
            {
                printf("图片未采集完成\n");
            }
            else if(resnum == DecodeLibNoInitError)
            {
                printf("库未初始化\n");
            }
        }
        else
        {
            printf("Decoding failed\n");
        }	
    }
 }

  /**
  * 双buff解码demo
  *
  */
 void DoubleBuffDecodeDemo(void)
 {
     //定义保存解码结果的数组
    uint8_t result[2100] = {0};
    int32_t resnum;    //返回解码结果数量，为0，表示解码失败，为-1表示解码数组偏小
    DecodeConfigTypeDef DecodeCfg = {0};  //设置解那种码
    DecodeResultTypeDef res = {.result = result, .maxn = 2100}; //初始化译码结构体
    
    //配置解码信息
    /**
     * 解码默认配置
     *
     * cfg->cfgCODE128 = (1u | (0u << DE_MIN_LEN) | (0xffu << DE_MAX_LEN));
     * cfg->cfgCODE39 = (1u | (0u << DE_MIN_LEN) | (0xffu << DE_MAX_LEN));
     * 
     * cfg->cfgQRCode = 1u | 1u << (DEQR_MISSINGCORNER) | (1u << DEQR_CURVE);
     * cfg->cfgGlobal = (1u << DEBAR_PRECISION);
     */
    DecodeConfigInit(&DecodeCfg);

    while(1) 
	{
        //建议添加此条判断，防止双buff都无效，造成卡死在while里
        if(GetDecodeBuffFlag() == FALSE)
        {
            //使能采图
            DecodeDcmiStart();
        }
        //检测采图是否完成，需要中断调用DCMI_CallBackFrame()函数
        while(!DecodeDcmiFinish())
        {
        }
        //采集完第一张图后解码前使能采集下一个buff图
        DecodeDcmiStart();
        
        //开始解码
        resnum = DecodeStart(&DecodeCfg, &res);

        if(resnum > 0)
        {
            printf("ID:%d\tAIMID:%s\n", res.id, res.AIM);
            for(int i = 0; i < res.resn; i++)
            {
                printf("%c", res.result[i]);
            }
            printf("\n");
            
            //解码成功将两个buff状态置为空闲，防止重复解码         
            CleanDecodeBuffFlag();

            beep(200);
        }
        else if(resnum < 0)
        {
            if(resnum == DecodeResOverflowError)
            {
                printf("解码结果溢出\n");
            }
            else if(resnum == DecodeImageNoDoneError)
            {
                printf("图片未采集完成\n");
            }
            else if(resnum == DecodeLibNoInitError)
            {
                printf("库未初始化\n");
            }
        }
        else
        {
            printf("Decoding failed\n");
        }       
	}
 }
