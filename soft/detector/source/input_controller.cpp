#include "input_controller.hpp"

#include <iostream>

InputController::InputController(Data& data, int argc, char* argv[])
    : data(data) {
    for (int i=1; i<argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'i':
                if (data.input_video_filename.size() > 0) {
                    show_help_and_exit();
                }
                data.input_video_filename = &argv[i][3];
                break;
            case 'o':
                if (data.output_video_filename.size() > 0) {
                    show_help_and_exit();
                }
                data.output_video_filename = &argv[i][3];
                break;
            case 'r':
                if (data.result_filename.size() > 0) {
                    show_help_and_exit();
                }
                data.result_filename = &argv[i][3];
                break;
            default:
                show_help_and_exit();
            }
        } else {
            show_help_and_exit();
        }
    }
}

void InputController::show_help_and_exit(void) {
    std::cout << "-i=<input_video_filename>" << std::endl;
    std::cout << "-o=<output_video_filename>" << std::endl;
    std::cout << "-r=<result_filename>" << std::endl;
    exit(-1);
}

void InputController::update(void) {
}
