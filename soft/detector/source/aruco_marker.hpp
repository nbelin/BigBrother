#ifndef ARUCOMARKER_HPP
#define ARUCOMARKER_HPP

#include <vector>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>
#include "position_marker.hpp"
#include "data.hpp"

class ArucoMarker {
public:
    bool getNextPos(cv::Mat& image, const struct Data& data, std::vector<PositionMarker>& nextPos);

private:
    static const unsigned int MIN_PM_HEIGHT = 10;
    static const unsigned int MAX_PM_HEIGHT = 200;
    bool convertCornersToPositionMarker(PositionMarker & pos); // use internally matching_markers and marker_corners
    std::vector<int> matching_markers;
    std::vector<std::vector<cv::Point2f> > marker_corners;
    std::vector<int> markers_ids;
};

#endif //ARUCOMARKER_HPP
