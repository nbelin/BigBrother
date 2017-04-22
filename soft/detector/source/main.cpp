#include "data.hpp"

#include "gui.hpp"
#include "color_chooser_gui.hpp"
#include "communication.hpp"
#include "video_controller.hpp"
#include "luv_controller.hpp"
#include "hsv_controller.hpp"
#include "detector_controller.hpp"
#include "input_controller.hpp"

#include <cereal/archives/json.hpp>

int main(int argc, char* argv[]) {
    Data data;
    data.gui_level = 0;
    data.color_choice = Color::HSV;

    /*
    std::cout << "CONFIG" << std::endl;
    cereal::JSONOutputArchive ar(std::cout);
    ar(CEREAL_NVP(config));
    std::cout << std::endl;
    */

    InputController input(data, argc, argv);
    VideoController video(data);
    HSVController hsv(data);
    LUVController luv(data);
    DetectorController detector(data);
    GUI gui(data);
    Communication comm(data);
    ColorChooserGUI ccgui(data);

    std::cout << "start loop" << std::endl;
    while(1) {
        input.update();
        video.update(); // get a new frame from camera
        hsv.update();
        luv.update();
        detector.update();
        gui.update();
        comm.update();
        ccgui.update();
    }
}
