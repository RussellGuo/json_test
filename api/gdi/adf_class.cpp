#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/cdefs.h>
#include <sys/mman.h>

#include <adf/adf.h>

class adf_t {
public:
    explicit adf_t():surf_base(nullptr) {
        init();
    }
    ~adf_t() {
        deinit();
    }

    bool getAdfInfo(void **fb_ptr, uint32_t *width, uint32_t *height, uint32_t *linelen) const {
        *fb_ptr = surf_base;
        if (surf_base == nullptr) {
            return false;
        }
        *width = intf_data.current_mode.hdisplay;
        *height = intf_data.current_mode.vdisplay;
        *linelen = surf_pitch;
        return true;
    }

    bool flip() const {
        int ret = adf_interface_simple_post(intf_fd, eng_id, intf_data.current_mode.hdisplay,intf_data.current_mode.vdisplay, format, surf_fd, surf_offset, surf_pitch, -1);
        if (ret >= 0) {
            close(ret);
        }
        return ret >= 0;
    }


private:
    const adf_id_t dev_idx = 0;
    struct adf_device dev;
    struct adf_interface_data intf_data;
    adf_id_t intf_id, eng_id;
    const uint32_t format = DRM_FORMAT_RGB565;
    int intf_fd;
    int surf_fd;
    __u32 surf_offset;
    __u32 surf_pitch;
    void *surf_base;

    bool init() {
        int err;
        err = adf_device_open(dev_idx, O_RDWR, &dev);
        if (err < 0) {
            fprintf(stderr, "opening adf device %u failed: %s\n", dev_idx, strerror(-err));
            return false;
        }

        err = adf_find_simple_post_configuration(&dev, &format, 1, &intf_id,  &eng_id);
        if (err < 0) {
            fprintf(stderr, "adf_find_simple_post_configuratione failed: %s\n", strerror(-err));
            return false;
        }
        err = adf_device_attach(&dev, eng_id, intf_id);
        if (err < 0 && err != -EALREADY) {
            fprintf(stderr, "adf_device_attach failed: %s\n", strerror(-err));
            return false;
        }

        intf_fd = adf_interface_open(&dev, intf_id, O_RDWR);
        if (intf_fd < 0) {
            fprintf(stderr, "adf_interface_open failed: %s\n", strerror(-intf_fd));
            return false;
        }

        err = adf_get_interface_data(intf_fd, &intf_data);
        if (err < 0) {
            fprintf(stderr, "adf_get_interface_data failed: %s\n", strerror(-err));
            return false;
        }
        fprintf(stderr, "intf_fd = %d, hdisplay = %d, vdisplay = %d\n", intf_fd, intf_data.current_mode.hdisplay, intf_data.current_mode.vdisplay);

        surf_fd = adf_interface_simple_buffer_alloc(intf_fd, intf_data.current_mode.hdisplay, intf_data.current_mode.vdisplay, format, &surf_offset, &surf_pitch);
        fprintf(stderr, "surf_fd, surf_offset, surf_pitch, %d, %u %u\n", surf_fd, surf_offset, surf_pitch);
        surf_base = mmap(nullptr, surf_pitch * intf_data.current_mode.vdisplay, PROT_WRITE, MAP_SHARED, surf_fd, surf_offset);
        if (surf_base == MAP_FAILED) {
            surf_base = nullptr;
            close(surf_fd);
            return false;
        }
        fprintf(stderr, "surf_base = %p\n", surf_base);
        adf_interface_blank(intf_fd,DRM_MODE_DPMS_OFF);
        adf_interface_blank(intf_fd,DRM_MODE_DPMS_ON);
        return true;

    }

public:
    bool deinit() {
        if (surf_base == nullptr || surf_base == MAP_FAILED) {
            return true;
        }
        int err = munmap(surf_base, surf_pitch * intf_data.current_mode.vdisplay);
        surf_base = nullptr;
        if (err != 0) {
            perror("umap surf_base");
            return false;
        }
        close(surf_fd);
        surf_fd = -1;
        close(intf_fd);
        intf_fd = -1;

        adf_free_interface_data(&intf_data);
        fprintf(stderr, "closed\n");
        return true;
    }

};

namespace  {
adf_t adf;
}

extern "C" bool initAdfDevice(void **fb_ptr, uint32_t *width, uint32_t *height, uint32_t *linelen)
{

    bool ret = adf.getAdfInfo(fb_ptr, width, height, linelen);
    return ret;
}

extern "C" void deinitAdfDevice(void)
{
    // adf.deinit();
}

extern "C" void AdfFlip(void)
{
    adf.flip();
}




