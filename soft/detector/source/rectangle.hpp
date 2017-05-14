#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "image.hpp"
#include "area.hpp"

#include "color_set.hpp"

class Rectangle {
public:
    Rectangle();
    Rectangle(const ColorSet& color_set);
private:
    ColorSet color_set;
    unsigned int startIvec;
    unsigned int endIvec;
    unsigned int startJvec;
    unsigned int endJvec;
    static const unsigned int MIN_AREA_HEIGHT = 3;
    static const unsigned int MIN_AREA_WIDTH = 8;
    static const unsigned int MAX_AREA_WIDTH = 300;
public:
    bool isPixelRightColor(int color_mode, const Image3D& image, unsigned int i, unsigned int j) const;
    void rateArea(Area& area);
    void expandArea(int color_mode, const Image3D& image, Image& mask, Area& area, unsigned int i, unsigned int j);
};


#endif //RECTANGLE_HPP
