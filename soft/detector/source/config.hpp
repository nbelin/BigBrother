#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "color_set.hpp"

#include <string>
#include <fstream>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

struct Beacon {
public:
    int top;
    int mid;
    int low;

public:
    Beacon(int top, int mid, int low)
        : top(top), mid(mid), low(low) {
    }

    template<class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(top));
        ar(CEREAL_NVP(mid));
        ar(CEREAL_NVP(low));
    }
};

struct Config {
public:
    int detector_id = 0;
    std::string rdv_ip = "127.0.0.1";
    unsigned short rdv_port = 1234;

    std::vector<ColorSet> color_set = {
//        ColorSet(Color(200, 66, 161), 30),
//        ColorSet(Color(47, 95, 85), 30),
//        ColorSet(Color(105, 140, 130), 20)
    };

    std::vector<Beacon> beacon {
        Beacon(0,1,2)
    };

public:
    template<class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(detector_id));
        ar(CEREAL_NVP(rdv_ip));
        ar(CEREAL_NVP(rdv_port));

        ar(CEREAL_NVP(color_set));
        ar(CEREAL_NVP(beacon));
    }
};

#endif//CONFIG_HPP
