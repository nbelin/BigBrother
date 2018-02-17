#include "calibrate_controller.hpp"
#include <iostream>


CalibrateController::CalibrateController(Data& data) : data(data) {
    cv::Point3f workingPoint;
    workingPoint.z = 0;
    double squareSize = 29.2 / 8;
    for (size_t i=0; i<7; ++i) {
        workingPoint.x = squareSize * i;
        for (size_t j=0; j<7; ++j) {
            workingPoint.y = squareSize * j;
            realPoints.push_back(workingPoint);
        }
    }
}

void CalibrateController::update(void) {
    if (data.method_choice != data.method_CALIBRATE || data.frame == nullptr) {
        return;
    }
    bool found = cv::findChessboardCorners(*(data.frame), cv::Size(7,7), workingPoints,
                                        CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
    if (found) {
        lastFoundPoints = workingPoints;
    }
    cv::drawChessboardCorners(*(data.frame), cv::Size(7,7), cv::Mat(workingPoints), found );
    showMat = data.frame->clone();
    showMat.resize(480,680);
    cv::imshow("video", showMat);
//    data.frame->resize(480,680);
//    cv::imshow("video", *(data.frame));
    int key = cv::waitKey(40);

    if (key == 10) { // enter
            std::cout << "Calibrating...\n";
            cv::calibrateCamera(realPointsVector, foundPoints, data.frame->size(), cameraMatrix, distCoef, rvecs, tvecs);
            std::cout << cameraMatrix << std::endl;
            std::cout << distCoef << std::endl;
            for (size_t i=0; i<rvecs.size(); ++i) {
                std::cout << rvecs[i] << std::endl;
            }
            for (size_t i=0; i<tvecs.size(); ++i) {
                std::cout << tvecs[i] << std::endl;
            }
    } else if (key >= 0 && lastFoundPoints.size() > 0) {
        std::cout << key << std::endl;
        foundPoints.push_back(lastFoundPoints);
        realPointsVector.push_back(realPoints);
        std::cout << foundPoints.size() << " recorded images.\n";
    }
}
