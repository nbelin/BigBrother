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
        //data.aruco_marker = new ArucoMarker;
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

        thread_aruco[0] = std::thread(&DetectorController::jobArucoMarkers, this, 0);
        thread_aruco[1] = std::thread(&DetectorController::jobArucoMarkers, this, 1);
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
        //data.aruco_marker->getNextPos(*(data.frame), data, data.pm);
        job_done[0] = false;
        job_done[1] = false;
        job_ready[0] = true;
        job_ready[1] = true;

        while (job_done[0] == false || job_done[1] == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    data.image.id++;
}

void DetectorController::jobArucoMarkers(int id) {
    // (id == 0) => look for allies (bottom of picture)
    // (id == 1) => look for enemies (top of picture)
    ArucoMarker aruco_marker;
    std::vector<PositionMarker> pm;
    cv::Rect rect(0, id * data.frame->rows / 2, data.frame->cols, data.frame->rows / 2); // to crop half of image
    while (true) {
        while (job_ready[id] == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        job_ready[id] = false;

        cv::Mat sub_image = (*(data.frame))(rect);
        aruco_marker.getNextPos(sub_image, data, pm);
        // markers 0 to 7 are allies
        // markers 8 to 15 are enemies
        //aruco_marker.markers_ids......
        job_done[id] = true;
    }
}
