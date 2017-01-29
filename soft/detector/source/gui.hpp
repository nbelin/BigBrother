#ifndef GUI_HPP
#define GUI_HPP

#include <opencv2/opencv.hpp>
#include "marker.hpp"
#include "image.hpp"

#include "data.hpp"

class GUI {
private:
    Data& data;

private:
    void addRectangle(const PositionMarker& pm);
    void addMask(const Image& msk);

public:
    GUI(Data& data);
    void update(void);
};

#endif//GUI_HPP
