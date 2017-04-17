#ifndef COLOR_HPP
#define COLOR_HPP

#include "util.hpp"
#include <cereal/cereal.hpp>

class Color {
public:
    static const int HSV = 1; //bit mask
    static const int LUV = 2; //bit mask

    unsigned char v1;
    unsigned char v2;
    unsigned char v3;

public:
    Color(void) {}
    Color(unsigned char v1, unsigned char v2, unsigned char v3) : v1(v1), v2(v2), v3(v3) {}
    bool operator<=(const Color& other) const {
        return v1 <= other.v1 && v2 <= other.v2 && v3 <= other.v3;
    }
    unsigned int luv_square_dist(const Color& other) const {
        unsigned int diff_u = absdiff(v2, other.v2);
        unsigned int square_diff_u = diff_u * diff_u;
        unsigned int diff_v = absdiff(v3, other.v3);
        unsigned int square_diff_v = diff_v * diff_v;
        return square_diff_u + square_diff_v;
    }

public:
    template<class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(v1));
        ar(CEREAL_NVP(v2));
        ar(CEREAL_NVP(v3));
    }
};

#endif//COLOR_HPP
