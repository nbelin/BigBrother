#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

#include "image.hpp"
#include "rectangle.hpp"
#include "list.hpp"
#include "marker.hpp"
#include "communication.hpp"
#include "../config/def_colors.hpp"
#include "../config/client.hpp"

#include "gui.hpp"

//#define VIDEO "sample/2015-06-06-172737.webm"
//#define VIDEO "sample/2015-06-06 17.29.40.mov"
//#define VIDEO "sample/2015-06-06-174454.webm"
//#define VIDEO "../../sample/test.mp4"
#define VIDEO 0

int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;
    std::cout << "GO" << std::endl;
    cv::VideoCapture cap(VIDEO); // open the default camera
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
    //Marker marker1(image, false, yellowRect, redRect, dummyRect);
    //Marker marker1(image, true, darkBlueRect, magentaRect, dummyRect);
    Marker marker1(image, true, darkBlueRect, greenRect, magentaRect);
    PositionMarker pm1(0);
#if 0
    Marker marker2(image, false, redRect, yellowRect, dummyRect);
    PositionMarker pm2(1);
    Marker marker3(image, true, greenRect, blueRect, dummyRect);
    PositionMarker pm3(2);
    Marker marker4(image, true, blueRect, greenRect, dummyRect);
    PositionMarker pm4(3);
#endif

    Communication comm(CAMERA_ID, SERVER_IP, SERVER_PORT);
    GUI gui;

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

#if 0 // simple conf: 1 marker to detect
        result = marker2.getNextPos(image, pm2);
        if (result) {
            comm.prepareMessage(&pm2);
        } else {
            std::cout << "POS " << count << std::endl;
            std::cout << "NOP (2)" << std::endl;
        }

        result = marker3.getNextPos(image, pm3);
        if (result) {
            comm.prepareMessage(&pm3);
        } else {
            std::cout << "POS " << count << std::endl;
            std::cout << "NOP (3)" << std::endl;
        }

        result = marker4.getNextPos(image, pm4);
        if (result) {
            comm.prepareMessage(&pm4);
        } else {
            std::cout << "POS " << count << std::endl;
            std::cout << "NOP (4)" << std::endl;
        }
#endif

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
