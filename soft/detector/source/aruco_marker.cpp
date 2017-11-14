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
    for (size_t i=0; i<nextPos.size(); ++i) {
        // which markers match?
        matching_markers.resize(0);
        for (size_t j=0; j<markers_ids.size(); ++j) {
            if (nextPos[i].pmID == markers_ids[j] / 4) {
                matching_markers.push_back(j);
            }
        }
        if (matching_markers.size() > 2) {
            //std::cout << "Warning: too much matching markers for one robot!\n";
        }
        int sumPixel = 0;
        int nbSum = 0;
        int maxJ = 0;
        int minJ = 9999;
        int maxI = 0;
        int minI = 9999;
        for (size_t j=0; j<matching_markers.size(); ++j) {
            const std::vector<cv::Point2f>& corners = marker_corners[matching_markers[j]];
            for (size_t k=0; k<corners.size(); ++k) {
                sumPixel += corners[k].x;
                nbSum++;
                if (maxJ < corners[k].x) {
                    maxJ = corners[k].x;
                }
                if (minJ > corners[k].x) {
                    minJ = corners[k].x;
                }
                if (maxI < corners[k].y) {
                    maxI = corners[k].y;
                }
                if (minI > corners[k].y) {
                    minI = corners[k].y;
                }
            }
        }
        if (nbSum > 0) {
            nextPos[i].x = sumPixel / nbSum;
            nextPos[i].size = maxJ - minJ;
            nextPos[i].minI = minI;
            nextPos[i].maxI = maxI;
            nextPos[i].confidence = 1;
        }
    }

    return true;
}


