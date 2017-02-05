#include "data.hpp"
#include "config.hpp"

#include "gui.hpp"
#include "color_chooser_gui.hpp"
#include "communication.hpp"
#include "video_controller.hpp"
#include "luv_controller.hpp"
#include "detector_controller.hpp"

int main(int argc, char* argv[]) {
    Data data;
    Config config;

    VideoController video(data, argc, argv);
    LUVController luv(data);
    DetectorController detector(data);
    GUI gui(data);
    Communication comm(data, config);
    ColorChooserGUI ccgui(data);

    std::cout << "start loop" << std::endl;
    while(1) {
        video.update(); // get a new frame from camera
        luv.update();
        detector.update();
        gui.update();
        comm.update();
        ccgui.update();
    }
}
