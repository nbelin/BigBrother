#ifndef COLOR_CHOOSER_GUI_HPP
#define COLOR_CHOOSER_GUI_HPP

#include "data.hpp"

class ColorChooserGUI {
private:
    Data& data;
public:
    ColorChooserGUI(Data& data);
    void update(void);
};

#endif//COLOR_CHOOSER_GUI_HPP
