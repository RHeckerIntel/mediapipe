#include <iostream>
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_videoio_inc.h"

#include <fcntl.h>              // For O_RDWR
#include <sys/ioctl.h>          // For ioctl
#include <linux/videodev2.h>
#include <unistd.h>             // For close
#include <set>                  // For std::set to track unique devices

void list_cameras() {
    std::set<std::string> listed_devices;
    for (int i = 0; i < 64; ++i) { // Check up to /dev/video63
        std::string device = "/dev/video" + std::to_string(i);
        int fd = open(device.c_str(), O_RDWR | O_NONBLOCK);
        if (fd == -1) {
            continue; // Device not found or can't be opened
        }

        v4l2_capability cap;
        if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
            close(fd);
            continue; // Failed to query capabilities
        }

        if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
            // Use bus_info to uniquely identify the device
            std::string device_id(reinterpret_cast<char*>(cap.bus_info));
            if (listed_devices.find(device_id) == listed_devices.end()) {
                std::cout << "Found camera: " << device << " - " << cap.card << std::endl;
                listed_devices.insert(device_id);
            }
        }

        close(fd);
    }
}
void video_stuff() {
  cv::VideoCapture cap;
  cv::Mat frame;

  int deviceID = 0;
  cap.open(deviceID);
  if (!cap.isOpened()) {
    std::cerr << "ERROR! Unable to open camera\n";
    return;
  }

  cap.read(frame);
  cv::imwrite("/data/output.png", frame);
  cap.release();
}


int main() {
  list_cameras();
}
