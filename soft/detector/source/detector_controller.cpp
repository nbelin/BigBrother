#include "detector_controller.hpp"
#include "../config/def_colors.hpp"

DetectorController::DetectorController(Data& data)
    : data(data) {

    if (data.method_choice == data.method_COLOR) {
        // This first (dummy) image is used to initialize buffers in Classes
        data.image = Image3D(data.frame->cols, data.frame->rows, NULL);

        for (size_t i=0; i<data.pm.size(); ++i) {
            data.marker.push_back(getMarker(data.image, data.pm[i].pmID));
        }
    } else {
        data.aruco_marker = new ArucoMarker;
        data.aruco_dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
        data.aruco_params = new cv::aruco::DetectorParameters;
        // data.aruco_params->doCornerRefinement = true;
        data.aruco_params->cornerRefinementMaxIterations = 500;
        // data.aruco_params->cornerRefinementWinSize = 1;
        data.aruco_params->cornerRefinementMinAccuracy = 0.001;
        // data.aruco_params->minMarkerPerimeterRate = 0.05;
        // data.aruco_params->maxMarkerPerimeterRate = 0.2;
        data.aruco_params->adaptiveThreshWinSizeMin = 10;
        // data.aruco_params->adaptiveThreshWinSizeStep = 3;
        data.aruco_params->adaptiveThreshWinSizeMax = 10;

    }

    data.image.id = 0;
}

void DetectorController::update(void) {
    if (data.image.id % 100 == 0) {
        std::cout << "NEXT POS " << data.image.id << std::endl; // this endl force flush from time to time
    }

    if (data.method_choice == data.method_COLOR) {
        for (size_t i=0; i<data.marker.size(); ++i) {
            if (data.marker[i].getNextPos(data.color_choice, data.image, data.pm[i])) {
                //std::cout << "Found at POS " << data.image.id << "\n;
                data.pm[i].display();
            } else {
                //std::cout << "POS " << data.image.id << "\n";
                //std::cout << "NOP (1)\n";
            }
        }
    } else {
        data.aruco_marker->getNextPos(*(data.frame), data, data.pm);
    }

    data.image.id++;
}
