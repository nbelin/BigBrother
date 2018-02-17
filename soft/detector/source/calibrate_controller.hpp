#ifndef CALIBRATE_CONTROLLER_HPP
#define CALIBRATE_CONTROLLER_HPP

#include "data.hpp"
#include <vector>
#include <opencv2/opencv.hpp>

class CalibrateController {
private:
    Data& data;
    std::vector<cv::Point2f> lastFoundPoints;
    std::vector<cv::Point2f> workingPoints;
    std::vector<std::vector<cv::Point2f>> foundPoints;
    std::vector<cv::Point3f> realPoints;
    std::vector<std::vector<cv::Point3f>> realPointsVector;
    cv::Mat showMat;
    cv::Mat cameraMatrix;
    cv::Mat distCoef;
    std::vector<cv::Mat> rvecs;
    std::vector<cv::Mat> tvecs;

public:
    CalibrateController(Data& data);

    void update(void);
};

#endif //CALIBRATE_CONTROLLER_HPP
