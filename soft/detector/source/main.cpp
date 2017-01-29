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

int main(int argc, char* argv[]) {
    cv::VideoCapture cap = open_cam(argc, argv) ;
    Communication comm(CAMERA_ID, SERVER_IP, SERVER_PORT);
    GUI gui;

    if(!cap.isOpened()) {  // check if we succeeded
        std::cout << "Failed to open video" << std::endl;
        return -1;
    }

    cv::Mat frame;
    cv::Mat hsv;
    cap >> frame;

    // This first (dummy) image is used to initialize buffers in Classes
    Image3D image(frame.cols, frame.rows, NULL);

    Rectangle dummyRect;
    Marker marker1(image, true, darkBlueRect, greenRect, magentaRect);
    PositionMarker pm1(0);

    std::cout << "start loop" << std::endl;
    int count = 0;
    while(1) {
        cap >> frame; // get a new frame from camera
        cv::cvtColor(frame, hsv, CV_BGR2Luv);
        image.setData(hsv.data);
        image.id = count;

        if (count % 20 == 0)
            std::cout << "NEXT POS " << count << std::endl;
        bool result;

        result = marker1.getNextPos(image, pm1);
        if (result) {
            comm.prepareMessage(&pm1);
        } else {
            std::cout << "POS " << count << std::endl;
            std::cout << "NOP (1)" << std::endl;
        }

        gui.setFrame(frame);
        // debug detected pixels for given marker/color
        gui.addMask(marker1.masks[0]);
        gui.addMask(marker1.masks[1]);
        gui.addMask(marker1.masks[2]);
        // show detected markers
        gui.addRectangle(pm1);

        // show the final image
        gui.update();

        count++;

        comm.sendMessage();
        comm.resetMessage();
    }
}
