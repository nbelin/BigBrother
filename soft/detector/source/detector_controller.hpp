#ifndef DETECTOR_CONTROLLER_HPP
#define DETECTOR_CONTROLLER_HPP

#include "data.hpp"

class DetectorController {
private:
    Data& data;
public:
    DetectorController(Data& data);

    void update(void);
};

#endif//DETECTOR_CONTROLLER_HPP
