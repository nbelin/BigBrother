#ifndef DATA_HPP
#define DATA_HPP

#include "image.hpp"
#include "position_marker.hpp"

#include <vector>

#include <opencv2/opencv.hpp>

struct Data {
    cv::Mat frame;
    Image image;
    std::vector<PositionMarker> pm;
};

#endif//DATA_HPP
