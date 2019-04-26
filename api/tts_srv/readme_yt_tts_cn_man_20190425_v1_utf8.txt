1. 版本更新
此次发布的版本支持联网激活：

-  针对X64/Linux(UBuntu)的lib为libyt_tts_cn_man_x64_300_net.a
- 针对ARM/Linux的lib为libyt_tts_cn_man_arm_300_net.a

内存依然需要300KB, 联网过程基于socket通信。
我司服务器下发的信息保存在：
- /data/yt_device_info_1.dat
- /data/yt_device_info_2.dat

两个文件中，请确保 /data 目录事先存在。

在设备没有激活之前， tts引擎会播报 “语音播报尚未开启，请联网激活” 的提示语。
数据文件yt_tts_man_data_one.dat有更新，主要是进一步缩短响应时间。 yt_tts_man_data_two_01.dat同最近版本，没有更新。

2. API更新
yt_tts_interface_300.h中新增了一个API:
	yt_tts_set_cb_print_info_300(void (*print_info)(char *strMessage,unsigned int *pTime))

可以用来打印一些引擎运行的TRACE信息（可酌情调用），以免需要调试。

回调函数print_info(char *strMessage,unsigned int *pTime)的参考写法：

void print_info(char *strMessage,unsigned int *pTime)
{
	printf("[TTS_INFO]: %s\r\n",strMessage);
	*pTime = 0;
}


其它API的用法没有更新。

3. Demo code
可以用先前的版本，原有的API用法不变。




