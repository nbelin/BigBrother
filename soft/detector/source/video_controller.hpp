#ifndef VIDEO_CONTROLER_HPP
#define VIDEO_CONTROLER_HPP

#ifdef RASPBERRY_PI
#define RASPICAM
#endif //RASPBERRY_PI

#include "data.hpp"

#include <thread>
#include <string>
#include <vector>
#include <chrono>
#ifdef RASPICAM
#include <raspicam/raspicam_cv.h>
#endif

class VideoController {
private:
    Data& data;
    cv::VideoCapture cap;
#ifdef RASPICAM
    raspicam::RaspiCam_Cv raspicap;
#endif
    cv::VideoWriter writer;
    cv::VideoCapture open_cam(std::string filename);
    std::thread thread;
    std::thread thread_writer;
    std::vector<cv::Mat> workingMats;
    int lastMatId;
    int readyMatId;
    static const int NBWORKMATS = 12;
    bool captureDefaultCam;
public:
    VideoController(Data& data);

    void update(void);

    void jobGetImage(void);
    void jobSaveImage(void);
};

#endif//VIDEO_CONTROLER_HPP
