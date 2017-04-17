#ifndef INPUT_CONTROLER_HPP
#define INPUT_CONTROLER_HPP

#include "data.hpp"

class InputController {
private:
    Data& data;
    void show_help_and_exit(const char * errMsg = nullptr);
public:
    InputController(Data& data, int argc, char* argv[]);

    void update(void);
};

#endif//INPUT_CONTROLER_HPP
