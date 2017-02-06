#ifndef DATA_HPP
#define DATA_HPP

#include "image.hpp"
#include "marker.hpp"

#include <vector>

#include <opencv2/opencv.hpp>

struct Data {
    cv::Mat frame;
    cv::Mat luv;
    cv::Mat hsv;
    Image3D image = {0,0,NULL};
    std::vector<PositionMarker> pm;
    std::vector<Marker> marker;
};

#endif//DATA_HPP
