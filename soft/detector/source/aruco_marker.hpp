#ifndef ARUCOMARKER_HPP
#define ARUCOMARKER_HPP

#include <vector>
#include <opencv2/aruco.hpp>
#include "position_marker.hpp"

class ArucoMarker {
public:
    ArucoMarker();
    bool getNextPos(cv::Mat& image, cv::Ptr<cv::aruco::Dictionary> dict, std::vector<PositionMarker> &nextPos);

private:
    static const unsigned int MIN_PM_HEIGHT = 10;
    static const unsigned int MAX_PM_HEIGHT = 200;
    std::vector<PositionMarker> previousPos;
};

#endif //ARUCOMARKER_HPP
