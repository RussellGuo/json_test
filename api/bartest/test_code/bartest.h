//
// Spreadtrum Barcode Tester
//
// for linux
//
#ifndef _Barcode_20190224_H__
#define _Barcode_20190224_H__

//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "cmr_common.h"
extern int flag;   //test

struct client_t {
    int reserved;
};

typedef struct img_info{
    unsigned int width;
    unsigned int height;
    unsigned int format;
    void *data;
}img_info_t;


typedef struct get_range {
 cmr_s32 cur_val;
 cmr_s32 max_val;
 cmr_s32 min_val;
} get_range_t;

struct mini_lib_ops {
    void  (*set_contrast)(struct minicamera_context *ctx, uint8_t contrast_set);
    void  (*get_contrast)(struct minicamera_context *ctx, get_range_t *contrast_range);
    void  (*set_shutter)(struct minicamera_context *ctx, int shutter_set);
    void  (*get_shutter)(struct minicamera_context *ctx, get_range_t *shutter_range);
    void  (*set_gain)(struct minicamera_context *ctx, int gain_set);
    void  (*get_gain)(struct minicamera_context *ctx, get_range_t *gain_range);
};

struct minicamera_context {
    oem_module_t *oem_dev;
    cmr_handle oem_handle;
    unsigned int camera_id;
    unsigned int width;
    unsigned int height;
    unsigned int prev_format;
    struct client_t client_data;
    struct mini_lib_ops lib_ops;
    void *lib_handle;
};
extern struct minicamera_context cxt;
int Cam_Init(void);
int Cam_DeInit(void);
bool mini_get_img_info(struct minicamera_context *cxt,img_info_t *out_param_ptr);
/*第二个参数是缩放比例，最大到4X，第三个参数是宽高比，宽高比不同于全尺寸的话，会裁剪*/
int  mini_set_CropRegion(struct minicamera_context *cxt,float zoomRatio,float aspect_ratio);

void dlmalloc_stats(void);
int  dlmalloc_trim(size_t);

#define MALLOC_STAT() do { dlmalloc_trim(0); fprintf(stderr, "malloc stats at %s:%d\n", __func__, __LINE__); dlmalloc_stats(); } while (0)

#ifdef __cplusplus
}
#endif // __cplusplus
//-----------------------------------------------------------------------------

#endif // _Barcode_20190224_H__
