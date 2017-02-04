#ifndef VIDEO_CONTROLER_HPP
#define VIDEO_CONTROLER_HPP

#include "data.hpp"

class VideoController {
private:
    Data& data;
    cv::VideoCapture cap;
public:
    VideoController(Data& data, int argc, char* argv[]);

    void update(void);
};

#endif//VIDEO_CONTROLER_HPP
