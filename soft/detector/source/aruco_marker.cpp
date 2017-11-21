#include "aruco_marker.hpp"
#include <iostream>

bool ArucoMarker::getNextPos(cv::Mat& image, const struct Data &data, std::vector<PositionMarker> &nextPos) {
    for (size_t i=0; i<nextPos.size(); ++i) {
        nextPos[i].reset();
    }

    // need to reset vectors?
    cv::aruco::detectMarkers(image, data.aruco_dict, marker_corners, markers_ids);//, data.aruco_params);

//    if (nextPos[0].pmID == 0) {
//        cv::aruco::drawDetectedMarkers(image, marker_corners, markers_ids);
//        cv::imshow("aruco", image);
//        cv::waitKey(10);
//    }

    // compute pm...
    for (size_t i=0; i<nextPos.size(); ++i) {
        // which markers match?
        matching_markers.resize(0);
        for (size_t j=0; j<markers_ids.size(); ++j) {
            if (nextPos[i].pmID == markers_ids[j] / 4) {
                matching_markers.push_back(j);
            }
        }
        convertCornersToPositionMarker(nextPos[i]);
    }

    return true;
}


/*
 * The corners are given in this order by OpenCV:
 *     -->   x
 *  0              1
 *   +------------+
 * | |            |
 * v |            |
 *   |            |
 * y |            |
 *   +------------+
 *  3              2
 */

bool ArucoMarker::convertCornersToPositionMarker(PositionMarker & pos) {

    if (matching_markers.size() > 2) {
        //std::cout << "Warning: too much matching markers for one robot!\n";
        // we should determine which marker is erroneous... (compare positions, etc.)
    }
    for (size_t j=0; j<matching_markers.size(); ++j) {
        const std::vector<cv::Point2f>& corners = marker_corners[matching_markers[j]];

        float left_height = corners[3].y - corners[0].y;
        float right_height = corners[2].y - corners[1].y;
        float mean_x = (corners[2].x + corners[3].x) / 2;

        if (left_height < right_height) {
            float ratio = right_height / left_height;
            pos.x = mean_x - 4.f * (1.f-ratio) * right_height; // try to find the actual center of the cube (approx. for now)
            pos.minI = corners[1].y;
            pos.maxI = corners[2].y;
            pos.size = right_height;
            pos.confidence = 1;
        } else {
            float ratio = left_height / right_height;
            pos.x = mean_x + 4.f * (1.f-ratio) * left_height; // try to find the actual center of the cube (approx. for now)
            pos.minI = corners[0].y;
            pos.maxI = corners[3].y;
            pos.size = left_height;
            pos.confidence = 1;
        }
        // for now do not take into account the 2nd detected marker
        return true;
    }
    return false;
}
