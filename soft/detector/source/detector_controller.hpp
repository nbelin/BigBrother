#ifndef DETECTOR_CONTROLLER_HPP
#define DETECTOR_CONTROLLER_HPP

#include <thread>
#include <chrono>
#include "data.hpp"

class DetectorController {
private:
    Data& data;

    std::thread thread_aruco[2];
    bool job_ready[2];
    bool job_done[2];
    void jobArucoMarkers(int id);
public:
    DetectorController(Data& data);

    void update(void);
};

#endif//DETECTOR_CONTROLLER_HPP
