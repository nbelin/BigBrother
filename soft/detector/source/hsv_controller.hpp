#ifndef HSV_CONTROLLER_HPP
#define HSV_CONTROLLER_HPP

#include "data.hpp"

class HSVController {
private:
    Data& data;
public:
    HSVController(Data& data)
        : data(data) {
    }

    void update(void) {
        cv::cvtColor(data.frame, data.hsv, CV_BGR2HSV);
        assert(data.hsv.data != nullptr);
        data.image.setData(data.hsv.data);
    }
};

#endif //HSV_CONTROLLER_HPP
