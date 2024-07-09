#ifndef CAMERA_INFO_H
#define CAMERA_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char device[256];
    char driver[16];
    char card[32];
    char bus_info[32];
    char version[16];
} CameraInfo;

typedef struct {
    char description[32];
    unsigned int pixelformat;
} FormatInfo;

typedef struct {
    unsigned int width;
    unsigned int height;
} FrameSizeInfo;

int list_cameras(CameraInfo *camera_info, int max_cameras);
int get_camera_formats(const char *device, FormatInfo *format_info, int max_formats);
int get_camera_frame_sizes(const char *device, unsigned int pixelformat, FrameSizeInfo *frame_size_info, int max_sizes);

#ifdef __cplusplus
}
#endif

#endif // CAMERA_INFO_H
