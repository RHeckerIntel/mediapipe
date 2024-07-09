#include "camera_info.h"
#include <iostream>
#include <fcntl.h>              // For O_RDWR
#include <unistd.h>             // For close
#include <sys/ioctl.h>          // For ioctl
#include <linux/videodev2.h>    // For V4L2
#include <cstring>              // For strcpy
#include <set>

extern "C" {

static void version_to_string(unsigned int version, char* version_str) {
    sprintf(version_str, "%u.%u.%u",
            (version >> 16) & 0xFF,
            (version >> 8) & 0xFF,
            version & 0xFF);
}

int list_cameras(CameraInfo *camera_info, int max_cameras) {
    std::set<std::string> listed_devices;

    int camera_count = 0;
    for (int i = 0; i < 64 && camera_count < max_cameras; ++i) {
        std::string device = "/dev/video" + std::to_string(i);
        int fd = open(device.c_str(), O_RDWR | O_NONBLOCK);
        if (fd == -1) {
            continue;
        }

        v4l2_capability cap;
        if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
            close(fd);
            continue;
        }

        std::string device_id(reinterpret_cast<char*>(cap.bus_info));
        bool new_device = listed_devices.find(device_id) == listed_devices.end();
        if (new_device && cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
            listed_devices.insert(device_id);

            strcpy(camera_info[camera_count].device, device.c_str());
            strcpy(camera_info[camera_count].driver, (char*)cap.driver);
            strcpy(camera_info[camera_count].card, (char*)cap.card);
            strcpy(camera_info[camera_count].bus_info, (char*)cap.bus_info);


            version_to_string(cap.version, camera_info[camera_count].version);
            camera_count++;
        }

        close(fd);
    }
    return camera_count;
}

int get_camera_formats(const char *device, FormatInfo *format_info, int max_formats) {
    int fd = open(device, O_RDWR | O_NONBLOCK);
    if (fd == -1) {
        return -1;
    }

    v4l2_fmtdesc fmtdesc;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmtdesc.index = 0;
    int format_count = 0;
    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) == 0 && format_count < max_formats) {
        strcpy(format_info[format_count].description, (char*)fmtdesc.description);
        format_info[format_count].pixelformat = fmtdesc.pixelformat;
        fmtdesc.index++;
        format_count++;
    }

    close(fd);
    return format_count;
}

int get_camera_frame_sizes(const char *device, unsigned int pixelformat, FrameSizeInfo *frame_size_info, int max_sizes) {
    int fd = open(device, O_RDWR | O_NONBLOCK);
    if (fd == -1) {
        return -1;
    }

    v4l2_frmsizeenum frmsize;
    frmsize.pixel_format = pixelformat;
    frmsize.index = 0;
    int size_count = 0;
    while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) == 0 && size_count < max_sizes) {
        if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
            frame_size_info[size_count].width = frmsize.discrete.width;
            frame_size_info[size_count].height = frmsize.discrete.height;
        } else if (frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) {
            frame_size_info[size_count].width = frmsize.stepwise.max_width;
            frame_size_info[size_count].height = frmsize.stepwise.max_height;
        }
        frmsize.index++;
        size_count++;
    }

    close(fd);
    return size_count;
}

} // extern "C"
