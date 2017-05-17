#ifndef VIDEO_CONTROLER_HPP
#define VIDEO_CONTROLER_HPP

#include "data.hpp"

#include <thread>
#include <string>

class VideoController {
private:
    Data& data;
    cv::VideoCapture cap;
    cv::VideoWriter writer;
    cv::VideoCapture open_cam(std::string filename);
    cv::Mat workingMat1;
    cv::Mat workingMat2;
    std::thread thread;
public:
    VideoController(Data& data);

    void update(void);

    static void jobGetImage(cv::VideoCapture * cap, cv::Mat * dst);
};

#endif//VIDEO_CONTROLER_HPP
