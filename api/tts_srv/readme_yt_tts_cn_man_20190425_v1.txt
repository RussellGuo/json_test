1. �汾����
�˴η����İ汾֧���������

-  ���X64/Linux(UBuntu)��libΪlibyt_tts_cn_man_x64_300_net.a
- ���ARM/Linux��libΪlibyt_tts_cn_man_arm_300_net.a

�ڴ���Ȼ��Ҫ300KB, �������̻���socketͨ�š�
��˾�������·�����Ϣ�����ڣ�
- /data/yt_device_info_1.dat
- /data/yt_device_info_2.dat

�����ļ��У���ȷ�� /data Ŀ¼���ȴ��ڡ�

���豸û�м���֮ǰ�� tts����Ქ�� ������������δ��������������� ����ʾ�
�����ļ�yt_tts_man_data_one.dat�и��£���Ҫ�ǽ�һ��������Ӧʱ�䡣 yt_tts_man_data_two_01.datͬ����汾��û�и��¡�

2. API����
yt_tts_interface_300.h��������һ��API:
	yt_tts_set_cb_print_info_300(void (*print_info)(char *strMessage,unsigned int *pTime))

����������ӡһЩ�������е�TRACE��Ϣ����������ã���������Ҫ���ԡ�

�ص�����print_info(char *strMessage,unsigned int *pTime)�Ĳο�д����

void print_info(char *strMessage,unsigned int *pTime)
{
	printf("[TTS_INFO]: %s\r\n",strMessage);
	*pTime = 0;
}


����API���÷�û�и��¡�

3. Demo code
��������ǰ�İ汾��ԭ�е�API�÷����䡣




