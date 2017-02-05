#ifndef COLOR_SET_HPP
#define COLOR_SET_HPP

#include "color.hpp"

#include <cereal/cereal.hpp>

class ColorSet {
private:
    Color center;
    unsigned int radius;

public:
    inline ColorSet(const Color& color, unsigned int radius)
        : center(color), radius(radius) {
    }

    inline bool contains(const Color& color) const {
        return color.v1 > 10 && center.luv_square_dist(color) < radius * radius;
    }

public:
    template<class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(center));
        ar(CEREAL_NVP(radius));
    }
};

#endif//COLOR_SET_HPP
