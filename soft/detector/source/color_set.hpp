#ifndef COLOR_SET_HPP
#define COLOR_SET_HPP

#include "color.hpp"

#include <cereal/cereal.hpp>

class ColorSet {
private:
    Color luv_center;
    unsigned int luv_radius;
    Color hsv_min;
    Color hsv_max;

public:
    inline ColorSet(const Color& luv_color, unsigned int luv_radius, const Color& hsv_min, const Color& hsv_max)
        : luv_center(luv_color), luv_radius(luv_radius), hsv_min(hsv_min), hsv_max(hsv_max) {
    }

    inline bool contains(int mode, const Color& color) const {
        if (mode == Color::HSV) {
            return hsv_min <= color && color <= hsv_max;
        }
        if (mode == Color::LUV) {
            return color.v1 > 10 && luv_center.luv_square_dist(color) < luv_radius * luv_radius;
        }
        std::cout << "Invalid color mode: " << mode << std::endl;
        return false;
    }

public:
    template<class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(luv_center));
        ar(CEREAL_NVP(luv_radius));
    }
};

#endif//COLOR_SET_HPP
