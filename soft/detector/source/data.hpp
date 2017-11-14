#ifndef DATA_HPP
#define DATA_HPP

#include "image.hpp"
#include "marker.hpp"
#include "aruco_marker.hpp"

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

struct Data {
    static const int method_COLOR = 1;
    static const int method_ARUCO = 2;
    cv::Mat * frame;
    cv::Mat luv;
    cv::Mat hsv;
    Image3D image = {0,0,NULL};
    std::vector<PositionMarker> pm;
    std::vector<Marker> marker;
    cv::Ptr<cv::aruco::Dictionary> aruco_dict;
    cv::Ptr<cv::aruco::DetectorParameters> aruco_params;
    int gui_level;
    std::string input_video_filename;
    std::string output_video_filename;
    std::string result_filename;
    int method_choice;
    int color_choice;
};

#endif//DATA_HPP
