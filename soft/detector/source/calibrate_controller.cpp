#include "calibrate_controller.hpp"
#include <iostream>


CalibrateController::CalibrateController(Data& data) : data(data) {
    lastCalibrationResult = -1;

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

    if (data.method_choice == data.method_CALIBRATE) {
        std::cout << "Starting calibration...\n <any key> to capture image\n <enter> to compute camera parameters\n";
    } else {
        loadCameraParams("camera.yml");
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
            lastCalibrationResult = cv::calibrateCamera(realPointsVector, foundPoints, data.frame->size(), cameraMatrix, distCoef, rvecs, tvecs);
            std::cout << "Final re-projection error: " << lastCalibrationResult << std::endl;
            saveCameraParams("camera.yml");
    } else if (key >= 0 && lastFoundPoints.size() > 0) {
//        std::cout << key << std::endl;
        foundPoints.push_back(lastFoundPoints);
        realPointsVector.push_back(realPoints);
        std::cout << foundPoints.size() << " recorded images.\n";
    }
}

void CalibrateController::saveCameraParams(const char *filename) {
    if (lastCalibrationResult < 0) {
        return;
    }
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);

    time_t rawTime;
    time(&rawTime);
    fs << "calibration_date" << asctime(localtime(&rawTime));
    fs << "calibration_result" << lastCalibrationResult;
    fs << "number_frames" << (int)foundPoints.size();
    fs << "camera_matrix" << cameraMatrix;
    fs << "dist_coeffs" << distCoef;
    fs << "rvecs" << rvecs;
    fs << "tvecs" << tvecs;

    std::cout << "Saved camera params into file " << filename << std::endl;
}

void CalibrateController::loadCameraParams(const char *filename) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);

    std::string date;
    int numberFrames;
    fs["calibration_date"] >> date;
    fs["calibration_result"] >> lastCalibrationResult;
    fs["number_frames"] >> numberFrames;
    fs["camera_matrix"] >> cameraMatrix;
    fs["dist_coeffs"] >> distCoef;
    fs["rvecs"] >> rvecs;
    fs["tvecs"] >> tvecs;

    std::cout << "Read camera parameters from " << filename << ": \n";
    std::cout << "calibration_date: " << date;
    std::cout << "number_frames: " << numberFrames << " \t calibration_result: " << lastCalibrationResult << std::endl;
}
