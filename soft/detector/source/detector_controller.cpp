#include "detector_controller.hpp"

DetectorController::DetectorController(Data& data)
    : data(data) {
    data.image.id = 0;
}

void DetectorController::update(void) {
    if (data.image.id % 100 == 0) {
        std::cout << "NEXT POS " << data.image.id << "\n";
    }

    for (size_t i=0; i<data.marker.size(); ++i) {
        if (data.marker[i].getNextPos(data.color_choice, data.image, data.pm[i])) {
            //std::cout << "Found at POS " << data.image.id << "\n;
            data.pm[i].display();
        } else {
            //std::cout << "POS " << data.image.id << "\n";
            //std::cout << "NOP (1)\n";
        }
    }
    data.image.id++;
}
