#include "detector_controller.hpp"

DetectorController::DetectorController(Data& data)
    : data(data) {
    data.image.id = 0;
}

void DetectorController::update(void) {
    if (data.image.id % 20 == 0)
        std::cout << "NEXT POS " << data.image.id << std::endl;

    if (data.marker[0].getNextPos(data.image, data.pm[0])) {
        //comm.prepareMessage(&data.pm[0]);
    } else {
        std::cout << "POS " << data.image.id << std::endl;
        std::cout << "NOP (1)" << std::endl;
    }
    data.image.id++;
}
