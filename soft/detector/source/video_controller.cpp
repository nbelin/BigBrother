#include "video_controller.hpp"

#include "../config/def_colors.hpp"

cv::VideoCapture open_cam(int argc, char* argv[]) {
    if(argc > 1) {
        std::cout << "Opening file : " << argv[1] << std::endl;
        return cv::VideoCapture(argv[1]);
    }
    else {
        std::cout << "Opening default camera" << std::endl;
        return cv::VideoCapture(0); // open the default camera
    }
}

VideoController::VideoController(Data& data, int argc, char* argv[])
    : data(data), cap(open_cam(argc, argv)) {

    if(!cap.isOpened()) {  // check if we succeeded
        std::cout << "Failed to open video" << std::endl;
       exit(-1);
    }

    cap >> data.frame;

    // This first (dummy) image is used to initialize buffers in Classes
    data.image = Image3D(data.frame.cols, data.frame.rows, NULL);
    data.marker.push_back(Marker(data.image, true, darkBlueSet, greenSet, magentaSet));
    data.pm.push_back(PositionMarker(0));
}

void VideoController::update(void) {
    cap >> data.frame;
}
