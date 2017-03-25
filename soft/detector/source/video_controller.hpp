#ifndef VIDEO_CONTROLER_HPP
#define VIDEO_CONTROLER_HPP

#include "data.hpp"

#include <string>

class VideoController {
private:
    Data& data;
    cv::VideoCapture cap;
    cv::VideoWriter writer;
    cv::VideoCapture open_cam(std::string filename);
public:
    VideoController(Data& data);

    void update(void);
};

#endif//VIDEO_CONTROLER_HPP
