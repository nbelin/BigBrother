#include "aruco_marker.hpp"
#include <iostream>
#include <cmath>

bool ArucoMarker::getNextPos(cv::Mat& image, const struct Data &data, std::vector<PositionMarker> &nextPos) {
    for (size_t i=0; i<nextPos.size(); ++i) {
        nextPos[i].reset();
    }

    // need to reset vectors?
    cv::aruco::detectMarkers(image, data.aruco_dict, marker_corners, markers_ids, data.aruco_params);
    cv::aruco::estimatePoseSingleMarkers(marker_corners, 65, data.cameraMatrix, data.distCoef, rvecs, tvecs);
    for (size_t i=0; i<markers_ids.size(); ++i) {
        cv::Mat rot_mat;
        cv::Rodrigues(rvecs[i], rot_mat);

        // transpose of rot_mat for easy columns extraction
        cv::Mat rot_mat_t = rot_mat.t();
        // transform along z axis
        double * rz = rot_mat_t.ptr<double>(2); // x=0, y=1, z=2
        double half_side = 35.;
        tvecs[i][0] +=  rz[0]*half_side;
        tvecs[i][1] +=  rz[1]*half_side;
        tvecs[i][2] +=  rz[2]*half_side;

//        std::cout << "dx " << rz[0] << std::endl;
//        std::cout << "dy " << rz[1] << std::endl;
//        std::cout << "dz " << rz[2] << std::endl << std::endl;

//        std::cout << std::atan2(rz[2], rz[0]) * 180. / M_PI << std::endl;

        cv::aruco::drawAxis(image, data.cameraMatrix, data.distCoef, rvecs[i], tvecs[i], 50.);
//        std::cout << "x " << rvecs[i][0] << std::endl;
//        std::cout << "y " << rvecs[i][1] << std::endl;
//        std::cout << "z " << rvecs[i][2] << std::endl << std::endl;
    }
    return true;

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

        if (matching_markers.size() == 0) {
            continue; // robot not detected
        }

        // for now keep only first matching marker for this robot
        for (size_t i=0; i<marker_corners[matching_markers[0]].size(); ++i) {
            std::cout << marker_corners[matching_markers[0]][i] << std::endl;
        }
        std::cout << data.cameraMatrix << std::endl;
        std::cout << data.distCoef << std::endl;
        std::cout << rvecs.size() << std::endl;
        std::cout << tvecs.size() << std::endl;
        cv::aruco::estimatePoseSingleMarkers(marker_corners[matching_markers[0]], 65, data.cameraMatrix, data.distCoef, rvecs, tvecs);
        cv::aruco::drawAxis(image, data.cameraMatrix, data.distCoef, rvecs, tvecs, 1);

        //convertCornersToPositionMarker(nextPos[i]);
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
        const int id = markers_ids[matching_markers[j]];

        const float left_height = corners[3].y - corners[0].y;
        const float right_height = corners[2].y - corners[1].y;
        const float mean_x = (corners[2].x + corners[3].x) / 2;
        const float bottom_width = corners[2].x - corners[3].x;

        if (left_height < right_height) {
            const float ratio = std::min(bottom_width / right_height, 1.f);
            pos.x = mean_x + 1.f * (1.f-ratio) * right_height; // try to find the actual center of the cube (approx. for now)
            pos.size = right_height;
            pos.orientation = int(360 + id * 90 + std::acos(ratio) * 180.f / M_PI) % 360;
            pos.minI = corners[1].y;
            pos.maxI = corners[2].y;
            pos.confidence = 1;
        } else {
            const float ratio = std::min(bottom_width / left_height, 1.f);
            pos.x = mean_x - 1.f * (1.f-ratio) * left_height; // try to find the actual center of the cube (approx. for now)
            pos.size = left_height;
            pos.orientation = int(360 + id * 90 - std::acos(ratio) * 180.f / M_PI) % 360;
            pos.minI = corners[0].y;
            pos.maxI = corners[3].y;
            pos.confidence = 1;
        }

        // for now do not take into account the (potential) 2nd detected marker
        return true;
    }
    return false;
}
