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

    lastMatId = 0;
    readyMatId = 0;
    workingMats.resize(NBWORKMATS);
    cap = open_cam(data.input_video_filename);

    if(!cap.isOpened()) {
        std::cout << "Failed to open video" << std::endl;
        std::cout << "On Rasp, try 'sudo modprobe bcm2835-v4l2' to enable PICam" << std::endl;
        exit(-1);
    }

//    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1*640);
//    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 2*480);

    thread = std::thread(&VideoController::jobGetImage, this, &cap);
    data.frame = & workingMats[NBWORKMATS - 1];
    cap >> * data.frame;
    lastMatId = NBWORKMATS - 1;

    // This first (dummy) image is used to initialize buffers in Classes
    data.image = Image3D(data.frame->cols, data.frame->rows, NULL);

    for (size_t i=0; i<data.pm.size(); ++i) {
        data.marker.push_back(getMarker(data.image, data.pm[i].pmID));
    }


    // Init VideoWriter to save the camera video to allow playback
    if (data.output_video_filename.size() > 0) {
        std::cout << "Opening (output) file : " << data.output_video_filename << std::endl;
        writer.open(data.output_video_filename, CV_FOURCC('M','P','E','G'), 30, data.frame->size());

        if(!writer.isOpened()) {
            std::cout << "Failed to open video" << std::endl;
            exit(-1);
        }

        writer << *data.frame;
    }
}

void VideoController::update(void) {
    while (lastMatId == readyMatId) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    data.frame = & workingMats[readyMatId];
    lastMatId = readyMatId;

    if (data.frame->empty()) {
        std::cout << "Video ends" << std::endl;
        exit(0);
    }
    if (writer.isOpened()) {
        writer << *data.frame;
    }
}

void VideoController::jobGetImage(cv::VideoCapture *cap) {
    while (true) {
        for (size_t i=0; i<NBWORKMATS; ++i) {
            *cap >> workingMats[i];
            readyMatId = i;
        }
    }
}
