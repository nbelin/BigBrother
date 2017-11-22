#include "video_controller.hpp"

#include "../config/def_colors.hpp"

cv::VideoCapture VideoController::open_cam(std::string filename) {
    if(filename.size() > 0) {
        std::cout << "Opening file : " << filename << "\n";
        captureDefaultCam = false;
        return cv::VideoCapture(filename.c_str());
    }
    else {
        std::cout << "Opening default camera\n";
        return cv::VideoCapture(0); // open the default camera
    }
}

VideoController::VideoController(Data& data)
    : data(data) {

    lastMatId = 0;
    readyMatId = 0;
    workingMats.resize(NBWORKMATS);
    captureDefaultCam = true;

    data.frame = & workingMats[NBWORKMATS - 1];

#ifdef RASPICAM
    if (data.input_video_filename.size() == 0) {
        raspicap.set(CV_CAP_PROP_FORMAT, CV_8UC3);
        raspicap.set(CV_CAP_PROP_FRAME_WIDTH, 960);
        raspicap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

        if(!raspicap.open()) {
            std::cout << "Failed to open video (raspicam)\n";
            exit(-1);
        }

        raspicap.grab();
        raspicap.retrieve(*data.frame);
    } else {
#endif
    cap = open_cam(data.input_video_filename);

    if(!cap.isOpened()) {
        std::cout << "Failed to open video\n";
        std::cout << "On Rasp, try 'sudo modprobe bcm2835-v4l2' to enable PICam\n";
        exit(-1);
    }

//    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1640);
//    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1232);
//    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1*640);
//    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 2*480);

    cap >> * data.frame;
#ifdef RASPICAM
    }
#endif
    //resize(*data.frame, *data.frame, cv::Size(640, 2*480));
    std::cout << "(" << data.frame->cols << ", " << data.frame->rows << ")\n";

    if (captureDefaultCam) {
        // if we are reading a file, don't read the frames in a separate thread (far too fast!)
        thread = std::thread(&VideoController::jobGetImage, this);
        lastMatId = NBWORKMATS - 1;
        readyMatId = NBWORKMATS - 1;
    }


    // Init VideoWriter to save the camera video to allow playback
    if (data.output_video_filename.size() > 0) {
        //data.output_video_filename = std::string("appsrc ! videoconvert ! omxh264enc ! h264parse ! mpegtsmux ! filesink location=") + data.output_video_filename;
        std::cout << "Opening (output) file: " << data.output_video_filename << "\n";
        if (data.output_video_codec.size() == 0) {
            //writer.open(data.output_video_filename, CV_FOURCC('M','P','E','G'), 30, data.frame->size());
            writer.open(data.output_video_filename, CV_FOURCC('M','J', 'P','G'), 30, data.frame->size());
        } else {
            std::cout << "Codec: " << data.output_video_codec << "\n";
            writer.open(data.output_video_filename, //cv::CAP_GSTREAMER,
                            CV_FOURCC(
                            data.output_video_codec[0],
                        data.output_video_codec[1],
                    data.output_video_codec[2],
                    data.output_video_codec[3]), 25, data.frame->size());
        }

        if(!writer.isOpened()) {
            std::cout << "Failed to open video\n";
            exit(-1);
        }

        thread_writer = std::thread(&VideoController::jobSaveImage, this);
        //writer << *data.frame;
    }
}

void VideoController::update(void) {
    if (captureDefaultCam) {
        while (lastMatId == readyMatId) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        data.frame = & workingMats[readyMatId];
        lastMatId = readyMatId;

    } else {
        // if we are reading a file, don't read the frames in a separate thread (far too fast!)
        cap >> * data.frame;

        if (data.frame->empty()) {
            std::cout << "Video ends\n";
            exit(0);
        }

        //resize(* data.frame, * data.frame, cv::Size(640, 2*480));
    }
}

void VideoController::jobGetImage() {
    while (true) {
        for (size_t i=0; i<NBWORKMATS; ++i) {

#ifdef RASPICAM
            raspicap.grab();
            raspicap.retrieve(workingMats[i]);
            // currently the pi camera is upside down, so rotate the image!
            cv::rotate(workingMats[i], workingMats[i], cv::ROTATE_180);
#else
            cap >> workingMats[i];
#endif

            if (workingMats[i].empty()) {
                std::cout << "Video ends\n";
                exit(0);
            }

            //resize(workingMats[i], workingMats[i], cv::Size(640, 2*480));
            readyMatId = i;
        }
    }
}

void VideoController::jobSaveImage() {
    int last_saved = NBWORKMATS - 1;
    while (true) {
    
        while (last_saved == readyMatId) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        last_saved = readyMatId;
        const cv::Mat & curMat = workingMats[last_saved];
        if (curMat.empty()) {
            continue;
        }
        writer << curMat;
        //std::cout << "last_saved=" << last_saved << std::endl;
    }
}
