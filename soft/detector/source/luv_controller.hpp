#ifndef LUV_CONTROLLER_HPP
#define LUV_CONTROLLER_HPP

#include "data.hpp"

class LUVController {
private:
    Data& data;
public:
    LUVController(Data& data)
        : data(data) {
    }

    void update(void) {
        cv::cvtColor(data.frame, data.hsv, CV_BGR2Luv);
        data.image.setData(data.hsv.data);
    }
};

#endif//LUV_CONTROLLER_HPP
