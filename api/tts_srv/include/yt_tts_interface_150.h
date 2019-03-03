#ifndef __YT_TTS_INTERFACE_150_HEADER__
#define __YT_TTS_INTERFACE_150_HEADER__

///////////////////////////////////////////////////////////////////////////
// VERSION SECTION  START
// 2011/04/20~      V2.0


// VERSION SECTION  END
///////////////////////////////////////////////////////////////////////////




//LANGUAGE ID PART: END
///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
//VOICE ID PART: START
#define YT_VOICE_ID_RESERVED_150 0
#define YT_VOICE_ID_FEMALE_150 1
#define YT_VOICE_ID_MALE_150 2
//VOICE ID PART: END
///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
//DATE FORMAT PART: START
#define YT_DATE_MM_DD_YYYY_150 0
#define YT_DATE_YYYY_MM_DD_150 1
#define YT_DATE_DD_MM_YYYY_150 2

//DATE FORMAT PART: END
///////////////////////////////////////////////////////////////////////////





///////////////////////////////////////////////////////////////////////////
//TEXT TYPE PART: START
#define YT_TEXT_TYPE_DEFAULT_150 0
#define YT_TEXT_TYPE_NAME_150 1
#define YT_TEXT_TYPE_NAME_APPENDIX_150 2
#define YT_TEXT_TYPE_MESSAGE_CONTENT_150 3
#define YT_TEXT_TYPE_MESSAGE_TITLE_150 4
#define YT_TEXT_TYPE_ICON_CAPTION_150 5
#define YT_TEXT_TYPE_DATE_150 6
#define YT_TEXT_TYPE_TIME_150 7
#define YT_TEXT_TYPE_TELEPHONE_NO_150 8
#define YT_TEXT_TYPE_DIGIT_150 9
#define YT_TEXT_TYPE_NUMBER_150 10
#define YT_TEXT_TYPE_HELP_150 11
#define YT_TEXT_TYPE_GPS_150 20
//TEXT TYPE PART: END
///////////////////////////////////////////////////////////////////////////


#define YT_TTS_MEM_SIZE_IN_BYTE 150*1024





///////////////////////////////////////////////////////////////////////////
//API SPECIFICATIONS: START

#ifdef __cplusplus
extern "C"
{
#endif

/*---------------------------------------------------------------------------------------
 |yt_tts_initialize_150(...)
 |PURPOSE
 |	This API intends to initialize TTS engine.
 |INPUT
 |	pMemBufferForEngine: buffer for TTS engine
 |	nMemSizeInByte: pMemBufferForEngine size in byte
 |
 |OUTPUT
 |	none
 |
 |RETURN VALUE
 |	It returns 0 if successful; else it returns -1.
 |
 |CODING
 |	YoungTone Inc.
 *--------------------------------------------------------------------------------------*/
int  yt_tts_initialize_150(char *pMemBufferForEngine, unsigned int nMemSizeInByte);
						    



/*-------------------------------------------------------------------------------
 |yt_tts_input_text_utf16_150(...)
 |PURPOSE
 |	This API intends to feed text in Unicode(UCS16) to TTS engine.
 |INPUT
 |	strText_U16: buffer for holding UTF16 text
 |	nTextLen_U16: number of character in strText_U16
 |	DATE_FORMAT: date format, which is defined in date format section
 |	nTextType: text type,which is defined in date format section. 
 |			    In general, user can set it to default value, 0
 |
 |
 |OUTPUT
 |	none
 |
 |RETURN VALUE
 |	It returns 0 if successful; else it returns -1.
 |
 |CODING
 |	YoungTone Tech.
 *-------------------------------------------------------------------------------*/
int  yt_tts_input_text_utf16_150(unsigned short *strText_U16,//[in]
							  unsigned int nTextLen_U16,//[in]
							  unsigned int DATE_FORMAT,
							  unsigned int nTextType);	
					

/*-------------------------------------------------------------------------------
 |yt_tts_input_text_mbcs_150(...)
 |PURPOSE
 |	This API intends to feed text in MBCS/ANSI to TTS engine.
 |INPUT
 |	strText: buffer for holding  text
 |	nTextLen: number of character in strText_U16
 |	DATE_FORMAT: date format, which is defined in date format section
 |	nTextType: text type,which is defined in date format section. 
 |			    In general, user can set it to default value, 0
 |
 |
 |OUTPUT
 |	none
 |
 |RETURN VALUE
 |	It returns 0 if successful; else it returns -1.
 |
 |CODING
 |	Shanghai YoungTone Tech.
 *-------------------------------------------------------------------------------*/
int  yt_tts_input_text_mbcs_150(char *strText,//[in]
							  unsigned int nTextLen,//[in]
							  unsigned int DATE_FORMAT,
							  unsigned int nTextType);





/*-------------------------------------------------------------------------------
 |yt_tts_free_resource_150(...)
 |PURPOSE
 |	This API intends to free resources occupied by TTS engine
 |	in most cases.
 |
 |INPUT
 | none
 |
 |OUTPUT
 |	none
 |
 |RETURN VALUE
 |	none
 |
 |CODING
 |	Shanghai YoungTone Tech.
 *-------------------------------------------------------------------------------*/
void yt_tts_free_resource_150();






/*-------------------------------------------------------------------------------
 |yt_tts_set_rate_150(...)
 |PURPOSE
 |	This API intends to set speech rate(speed).
 |
 |INPUT
 | nRateFactor: the target speech rate, which ranges 50~200. 
 |				100 is the normal rate. 50 is the fastest speed while 200 is the slowest one.
 |
 |OUTPUT
 |	none
 |
 |RETURN VALUE
 |	the acutal speech rate.
 |
 |CODING
 |	Shanghai YoungTone Tech.
 *-------------------------------------------------------------------------------*/
int yt_tts_set_rate_150(int nRateFactor);





/*-------------------------------------------------------------------------------
 |yt_tts_set_pitch_150(...)
 |PURPOSE
 |	This API intends to set pitch.
 |
 |INPUT
 | nPitchFactor: the target pitch factor, which ranges 50~200. 
 |				100 is the normal pitch. 50 is the lowest pitch while 200 is the highest one.
 |
 |OUTPUT
 |	none
 |
 |RETURN VALUE
 |	the acutal pitch.
 |
 |CODING
 |	Shanghai YoungTone Tech.
 *-------------------------------------------------------------------------------*/
int yt_tts_set_pitch_150(int nPitchFactor);




/*-------------------------------------------------------------------------------
 |yt_tts_set_volume_150(...)
 |PURPOSE
 |	This API intends to set volume.
 |
 |INPUT
 |nMaxVolume: the target volume factor, which ranges 0~32768. 
 |				32768 means the loudest speech while 0 means silence
 |
 |OUTPUT
 |	none
 |
 |RETURN VALUE
 |	the acutal volume
 |
 |CODING
 |	Shanghai YoungTone Tech.
 *-------------------------------------------------------------------------------*/
int yt_tts_set_volume_150(int nMaxVolume);





/*-------------------------------------------------------------------------------
 |yt_tts_utf8_to_utf16_150(...)
 |PURPOSE
 |	This API intends to conduct encoding conversion from UTF8 to UTF16
 |
 |INPUT
 |	strText_UTF8: buffer for the input text string in UTF8 encoding
 |	nTextLen_UTF8: length of strText_UTF8 in byte
 |
 |OUTPUT
 |	strText_U16: buffer for holding text in UTF16 encoding
 |	pTextLen_U16: pointer to character number of strText_U16
 |
 |RETURN VALUE
 |	none
 |
 |CODING
 |	Shanghai YoungTone Tech.
 *-------------------------------------------------------------------------------*/
void  yt_tts_utf8_to_utf16_150(char *strText_UTF8,
							unsigned int nTextLen_UTF8,
							unsigned short *strText_U16,
							unsigned int  *pTextLen_U16);





/*-------------------------------------------------------------------------------
 |yt_tts_get_speech_frame_150(...)
 |PURPOSE
 |	This API intends to get speech frame from TTS engine...
 |
 |BACKGROUND
 |	Several customers need to return speech frame by frame,  
 |	So we need to provide such kind of API.
 |
 |INPUT
 |	none
 |
 |OUTPUT
 |	pSpeechFrame: buffer for holding speech samples, which should be writable
 |				  Typical  pSpeechFrame contains 1000 short integer
 |	pSampleNumber: pointer to sample number of pSpeechFrame
 |
 |RETURN VALUE
 |	It returns a flag indicating TTS engine status.
 |		0: indicates the engine reaches text end, say all the text has been processed...
 |		1: indicates a sentence has been completed
 |		2: indicates the engine needs to continue
 |		10: indicates the engine reaches phrase boundary.
 |
 |CODING
 |	Shanghai YoungTone Tech.
 *-------------------------------------------------------------------------------*/
int yt_tts_get_speech_frame_150(short *pSpeechFrame, unsigned int *pSampleNumber);
												  






void yt_tts_stop_play_150();
void yt_tts_start_play_150();


int yt_tts_get_sampling_rate_150(void);












#ifdef __cplusplus
}
#endif


#endif
