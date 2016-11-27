#ifndef VIDEO_CONTROLER_HPP
#define VIDEO_CONTROLER_HPP

#include "data.hpp"

#include <thread>
#include <string>
#include <vector>
#include <chrono>

class VideoController {
private:
    Data& data;
    cv::VideoCapture cap;
    cv::VideoWriter writer;
    cv::VideoCapture open_cam(std::string filename);
    std::thread thread;
    std::vector<cv::Mat> workingMats;
    int lastMatId;
    int readyMatId;
    static const int NBWORKMATS = 8;
public:
    VideoController(Data& data);

    void update(void);

    void jobGetImage(cv::VideoCapture * cap);
};

#endif//VIDEO_CONTROLER_HPP
