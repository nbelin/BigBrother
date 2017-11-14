#include "aruco_marker.hpp"
#include <iostream>

bool ArucoMarker::getNextPos(cv::Mat& image, const struct Data &data, std::vector<PositionMarker> &nextPos) {
    for (size_t i=0; i<nextPos.size(); ++i) {
        nextPos[i].reset();
    }

    // need to reset vectors?
    cv::aruco::detectMarkers(image, data.aruco_dict, marker_corners, markers_ids, data.aruco_params);

    //cv::aruco::drawDetectedMarkers(image, marker_corners, markers_ids);
    //cv::imshow("aruco", image);
    //cv::waitKey(10);

    // compute pm...

    return true;
}


