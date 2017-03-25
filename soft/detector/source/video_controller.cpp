#include "video_controller.hpp"

#include "../config/def_colors.hpp"

cv::VideoCapture VideoController::open_cam(std::string filename) {
    if(filename.size() > 0) {
        std::cout << "Opening file : " << filename << std::endl;
        return cv::VideoCapture(filename.c_str());
    }
    else {
        std::cout << "Opening default camera" << std::endl;
        return cv::VideoCapture(0); // open the default camera
    }
}

VideoController::VideoController(Data& data)
    : data(data) {

    cap = open_cam(data.input_video_filename);

    if(!cap.isOpened()) {
        std::cout << "Failed to open video" << std::endl;
        exit(-1);
    }

    cap >> data.frame;

    // This first (dummy) image is used to initialize buffers in Classes
    data.image = Image3D(data.frame.cols, data.frame.rows, NULL);
    data.marker.push_back(Marker(data.image, true, darkBlueSet, greenSet, magentaSet));
    data.pm.push_back(PositionMarker(0));


    // Init VideoWriter to save the camera video to allow playback
    if (data.output_video_filename.size() > 0) {
        std::cout << "Opening (output) file : " << data.output_video_filename << std::endl;
        writer.open(data.output_video_filename, CV_FOURCC('M','P','E','G'), 30, data.frame.size());

        if(!writer.isOpened()) {
            std::cout << "Failed to open video" << std::endl;
            exit(-1);
        }

        writer << data.frame;
    }
}

void VideoController::update(void) {
    cap >> data.frame;
    writer << data.frame;
}
