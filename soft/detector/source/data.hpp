#ifndef DATA_HPP
#define DATA_HPP

#include "image.hpp"
#include "marker.hpp"

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>


struct Data {
    cv::Mat * frame;
    cv::Mat luv;
    cv::Mat hsv;
    Image3D image = {0,0,NULL};
    std::vector<PositionMarker> pm;
    std::vector<Marker> marker;
    int gui_level;
    std::string input_video_filename;
    std::string output_video_filename;
    std::string result_filename;
    int color_choice;
};

#endif//DATA_HPP
