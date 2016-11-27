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

    data.frame = & workingMats[NBWORKMATS - 1];

#ifdef RASPICAM
    raspicap.set(CV_CAP_PROP_FORMAT, CV_8UC3);
//    raspicap.set(CV_CAP_PROP_FRAME_WIDTH, 1640);
//    raspicap.set(CV_CAP_PROP_FRAME_HEIGHT, 1232);

    if(!raspicap.open()) {
        std::cout << "Failed to open video (raspicam)" << std::endl;
        exit(-1);
    }

    raspicap.grab();
    raspicap.retrieve(*data.frame);
#else
    cap = open_cam(data.input_video_filename);

    if(!cap.isOpened()) {
        std::cout << "Failed to open video" << std::endl;
        std::cout << "On Rasp, try 'sudo modprobe bcm2835-v4l2' to enable PICam" << std::endl;
        exit(-1);
    }

//    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1640);
//    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1232);
//    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1*640);
//    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 2*480);

    cap >> * data.frame;
#endif
    resize(*data.frame, *data.frame, cv::Size(640, 2*480));
    std::cout << "(" << data.frame->cols << ", " << data.frame->rows << ")" << std::endl;

    thread = std::thread(&VideoController::jobGetImage, this);
    lastMatId = NBWORKMATS - 1;
    readyMatId = NBWORKMATS - 1;

    // This first (dummy) image is used to initialize buffers in Classes
    data.image = Image3D(data.frame->cols, data.frame->rows, NULL);

    for (size_t i=0; i<data.pm.size(); ++i) {
        data.marker.push_back(getMarker(data.image, data.pm[i].pmID));
    }


    // Init VideoWriter to save the camera video to allow playback
    if (data.output_video_filename.size() > 0) {
        std::cout << "Opening (output) file : " << data.output_video_filename << std::endl;
        //writer.open(data.output_video_filename, CV_FOURCC('M','P','E','G'), 30, data.frame->size());
        writer.open(data.output_video_filename, CV_FOURCC('M','J', 'P','G'), 30, data.frame->size());

        if(!writer.isOpened()) {
            std::cout << "Failed to open video" << std::endl;
            exit(-1);
        }

        thread_writer = std::thread(&VideoController::jobSaveImage, this);
        //writer << *data.frame;
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
 //       writer << *data.frame;
    }
}

void VideoController::jobGetImage() {
    while (true) {
        for (size_t i=0; i<NBWORKMATS; ++i) {
#ifdef RASPICAM
            raspicap.grab();
            raspicap.retrieve(workingMats[i]);
#else
            cap >> workingMats[i];
#endif
            resize(workingMats[i], workingMats[i], cv::Size(640, 2*480));
            readyMatId = i;
        }
    }
}

void VideoController::jobSaveImage() {
    int last_saved = NBWORKMATS - 1;
    while (true) {
    
        while (last_saved == readyMatId) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        last_saved = readyMatId;
        const cv::Mat & curMat = workingMats[last_saved];
        if (curMat.empty()) {
            continue;
        }
        writer << curMat;
    }
}
