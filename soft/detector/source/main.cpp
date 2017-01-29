#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

#include "image.hpp"
#include "rectangle.hpp"
#include "marker.hpp"
#include "communication.hpp"
#include "../config/def_colors.hpp"
#include "../config/client.hpp"

#include "data.hpp"
#include "gui.hpp"

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

class VideoController {
private:
    Data& data;
    cv::VideoCapture cap;
public:
    VideoController(Data& data, int argc, char* argv[])
        : data(data), cap(open_cam(argc, argv)) {

        if(!cap.isOpened()) {  // check if we succeeded
            std::cout << "Failed to open video" << std::endl;
           exit(-1);
        }

        cap >> data.frame;

        // This first (dummy) image is used to initialize buffers in Classes
        data.image = Image3D(data.frame.cols, data.frame.rows, NULL);
        data.marker.push_back(Marker(data.image, true, darkBlueRect, greenRect, magentaRect));
        data.pm.push_back(PositionMarker(0));
    }

    void update(void) {
        cap >> data.frame;
    }
};

class LUVController {
private:
    Data& data;
public:
    LUVController(Data& data)
        : data(data) {
    }

    void update(void) {
        cv::cvtColor(data.frame, data.hsv, CV_BGR2Luv);
        data.image.setData(data.hsv.data);
    }
};

class DetectorController {
private:
    Data& data;
public:
    DetectorController(Data& data)
        : data(data) {
        data.image.id = 0;
    }

    void update(void) {
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
};

int main(int argc, char* argv[]) {
    Data data;

    VideoController video(data, argc, argv);
    LUVController luv(data);
    DetectorController detector(data);
    GUI gui(data);
    Communication comm(data, CAMERA_ID, SERVER_IP, SERVER_PORT);

    std::cout << "start loop" << std::endl;
    while(1) {
        video.update(); // get a new frame from camera
        luv.update();
        detector.update();
        gui.update();
        comm.update();
    }
}
