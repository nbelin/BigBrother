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
        cv::cvtColor(data.frame, data.luv, CV_BGR2Luv);
        assert(data.luv.data != nullptr);
        data.image.setData(data.luv.data);
    }
};

#endif//LUV_CONTROLLER_HPP
