
#define LOG_TAG "minicamapi"
    
#include <dlfcn.h>
#include <utils/Log.h>
#include "cmr_types.h"
#include "cmr_common.h"
#include "sprd_ion.h"
#include "MemIon.h"
#include <pthread.h>

typedef struct get_range {
 cmr_s32 cur_val;
 cmr_s32 max_val;
 cmr_s32 min_val;
} get_range_t;

struct client_t {
    int reserved;
};

#define SET_PARM(h, x, y, z)                                                   \
    do {                                                                       \
        if (NULL != h && NULL != h->ops)                                       \
            h->ops->camera_set_param(x, y, z);                                 \
    } while (0)

typedef struct minicamera_context {
    oem_module_t *oem_dev;
    cmr_handle oem_handle;
    unsigned int camera_id;
    unsigned int width;
    unsigned int height;
    struct client_t client_data;
}mini_ctx;

typedef struct mini_ops {
    cmr_int (*set_contrast)(mini_ctx* ctx, uint8_t contrast_set);
    cmr_int (*get_contrast)(mini_ctx* ctx, get_range_t *contrast_range);
    cmr_int (*set_shutter)(mini_ctx* ctx, int shutter_set);
    cmr_int (*get_shutter)(mini_ctx* ctx, get_range_t *shutter_range);
    cmr_int (*set_gain)(mini_ctx* ctx, int gain_set);
    cmr_int (*get_gain)(mini_ctx* ctx, get_range_t *gain_range);
}mini_ops_t;
extern "C"{
cmr_int set_contrast(mini_ctx* cxt, uint8_t contrast_set) {
    if (cxt) {
    SET_PARM(cxt->oem_dev, cxt->oem_handle, CAMERA_PARAM_CONTRAST, (uint32_t)contrast_set);
    }
    return 0;
}

cmr_int get_contrast(mini_ctx* cxt, get_range_t *contrast_range) {
    if (cxt) {
        cxt->oem_dev->ops->camera_ioctrl(cxt->oem_handle, CAMERA_IOCTRL_GET_CONTRAST_VAL, contrast_range);
        CMR_LOGD("GET_CONTRAST_VAL cur: 0x%x, max: 0x%x, min: 0x%x", contrast_range->cur_val, contrast_range->max_val, contrast_range->min_val);
    }
    return 0;
}

cmr_int set_shutter(mini_ctx* cxt, int shutter_set) {
    if (cxt) {
        cxt->oem_dev->ops->camera_ioctrl(cxt->oem_handle, CAMERA_IOCTRL_SET_SHUTTER_VAL, &shutter_set);
    }
    return 0;
}

cmr_int get_shutter(mini_ctx* cxt, get_range_t *shutter_range) {
    if (cxt) {
        cxt->oem_dev->ops->camera_ioctrl(cxt->oem_handle, CAMERA_IOCTRL_GET_SHUTTER_VAL, shutter_range);
        CMR_LOGD("yuzan GET_SHUTTER_VAL cur: %d, max: %d, min: %d", shutter_range->cur_val, shutter_range->max_val, shutter_range->min_val);
    }
    return 0;
}

cmr_int set_gain(mini_ctx* cxt, int gain_set) {
    if (cxt) {
        cxt->oem_dev->ops->camera_ioctrl(cxt->oem_handle, CAMERA_IOCTRL_SET_GAIN_VAL, &gain_set);
    }
    return 0;
}

cmr_int get_gain(mini_ctx* cxt, get_range_t *gain_range) {
    if (cxt) {
        cxt->oem_dev->ops->camera_ioctrl(cxt->oem_handle, CAMERA_IOCTRL_GET_GAIN_VAL, gain_range);
        CMR_LOGD("GET_GAIN_VAL cur: %d, max: %d, min: %d", gain_range->cur_val, gain_range->max_val, gain_range->min_val);
    }
    return 0;
}
static mini_ops_t mini_module_ops {
    set_contrast, get_contrast, set_shutter, get_shutter,
    set_gain, get_gain
};
}
