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
        if (data.method_choice != data.method_COLOR || (data.color_choice & Color::LUV) == 0) {
            return;
        }
        cv::cvtColor(*data.frame, data.luv, CV_BGR2Luv);
        assert(data.luv.data != nullptr);
        data.image.setData(data.luv.data);
    }
};

#endif//LUV_CONTROLLER_HPP
