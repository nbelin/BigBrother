#include "aruco_marker.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

ArucoMarker::ArucoMarker() {
    previousPos.resize(4);
}

bool ArucoMarker::getNextPos(cv::Mat& image, cv::Ptr<cv::aruco::Dictionary> dict, std::vector<PositionMarker> &nextPos) {
    for (size_t i=0; i<nextPos.size(); ++i) {
        nextPos[i].reset();
    }

    std::vector<std::vector<cv::Point2f> > marker_corners;
    std::vector<int> markers_ids;

    cv::aruco::detectMarkers(image, dict, marker_corners, markers_ids);

    //cv::aruco::drawDetectedMarkers(image, marker_corners, markers_ids);
    //cv::imshow("aruco", image);
    //cv::waitKey(10);

    for (size_t i=0; i<nextPos.size(); ++i) {
        previousPos[i] = nextPos[i];
    }
    return true;
}


