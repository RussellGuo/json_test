/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "bartest.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char c;  
    img_info_t imginfo;     
    sleep(1);
    usleep(100000);
for (;;)
{   
    fprintf(stderr, "please press key:\r\n");
    fprintf(stderr, "   1: camera init\r\n");
    fprintf(stderr, "   2: get imageinfo\r\n"); 
    fprintf(stderr, "   3: set_CropRegion\r\n");
    fprintf(stderr, "   4: set_gain\r\n");
    fprintf(stderr, "   5: set_shutter\r\n");
    fprintf(stderr, "   6: camera deinit\r\n");  
    c = getchar();
    switch(c) {
    case '1':  
    Cam_Init();
     break;
    case '2':    
     mini_get_img_info(&cxt,&imginfo);
     fprintf(stdout,"yuzan imginfo: w %d,h %d,fmt %d,data vir_addr 0x%x\n",
		imginfo.width,imginfo.height,imginfo.format,(unsigned int)imginfo.data);
    flag = 1;    //test
    usleep(100000);
      break;
    case '3':   
      for(int i = 1; i < 20; i ++){
     if(i > 9){
	 //test aspect ratio, zoom ratio defalut 1.00000
          mini_set_CropRegion(&cxt,1.00000,(float)(1+0.1*(i-10)));
     }else{
        fprintf(stderr,"yuzan4 here\n");
         //test zoom ratio,aspect ratio default 4:3
        mini_set_CropRegion(&cxt,1.00000+0.1*i,(float)cxt.width/cxt.height);
     }
        usleep(100000);
    }
      break;
    case '4':
    cxt.lib_ops.set_gain(&cxt,900);
    get_range_t gain_range;
    cxt.lib_ops.get_gain(&cxt,&gain_range);
    CMR_LOGI("test gain %d",gain_range.cur_val);
    fprintf(stdout,"test gain %d\n",gain_range.cur_val);
    flag = 1;    //test

    usleep(100000);
    //test gain value
    cxt.lib_ops.set_gain(&cxt,500);
    memset(&gain_range,0,sizeof(gain_range));
    cxt.lib_ops.get_gain(&cxt,&gain_range);
    CMR_LOGI("test gain %d",gain_range.cur_val);
    fprintf(stdout,"test gain %d\n",gain_range.cur_val);
    flag = 1;    //test
    usleep(100000);
     break;
    case '5': 
   //test exposure value
    cxt.lib_ops.set_shutter(&cxt,500);
    get_range_t shutter_range;
    cxt.lib_ops.get_shutter(&cxt,&shutter_range);
    CMR_LOGI("test shutter %d",shutter_range.cur_val);
    fprintf(stdout,"test shutter %d\n",shutter_range.cur_val);

    usleep(100000);
    cxt.lib_ops.set_shutter(&cxt,2000);
    cxt.lib_ops.get_shutter(&cxt,&shutter_range);
    CMR_LOGI("test shutter %d",shutter_range.cur_val);
    fprintf(stdout,"test shutter %d\n",shutter_range.cur_val);
    flag = 1;    //test
    usleep(100000); 
     break;
    case '6':  
     Cam_DeInit();
      break;
   }
}
 return 0;
}

