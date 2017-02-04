#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

#include "image.hpp"
#include "rectangle.hpp"
#include "marker.hpp"
#include "communication.hpp"
#include "../config/client.hpp"

#include "data.hpp"
#include "gui.hpp"
#include "video_controller.hpp"
#include "luv_controller.hpp"
#include "detector_controller.hpp"

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
