#ifndef POSITION_MARKER_HPP
#define POSITION_MARKER_HPP

#include <iostream>

class PositionMarker {
public:
    PositionMarker(unsigned short pmID = 0) : pmID(pmID) {
        reset();
    }
    unsigned short pmID;
    unsigned short imageID;
    unsigned short x;
    unsigned short size;
    float angle;
    unsigned short distance;
    short orientation;
    unsigned short minI;
    unsigned short maxI;
    float confidence;
    short dx;
    short dsize;
    void display() const {
        std::cout << "pmID=" << pmID << " imID=" << imageID << " \tx=" << x << " \tsize=" << size << "\n";
        std::cout << "distance=" << distance << " \tangle=" << angle << " \torientation=" << orientation << "\n";
        std::cout << "minI=" << minI << " \tmaxI=" << maxI << "\n";
        std::cout << "confidence=" << confidence << " \tdx=" << dx << " \tdsize=" << dsize << "\n";
    }
    void reset() {
        imageID = 0;
        x = 0;
        size = 0;
        angle = 0;
        distance = 0;
        orientation = 0;
        minI = 0;
        maxI = 0;
        confidence = 0;
        dx = 0;
        dsize = 0;
    }
    bool hasBeenFound() const {
        return confidence > 0;
    }
    bool toWorld(float * angle, unsigned short * distance) const {
        if (angle == nullptr || distance == nullptr) {
            return false;
        }
        if (this->distance > 0) {
            // we already have the information needed
            *angle = this->angle;
            *distance = this->distance;
        }
        /*
         * - angle from -1 (90° to right) to 1 (90° to left)
         * - distance in mm
         * Some pixels have been associated to angles:
         *  632 = -45°     =>  0.5
         *  556 = -33.7°   =>  0.374
         *  445 = -18.4°   =>  0.204
         *  414 = -14°     =>  0.156
         *  316 =   0°     =>  0
         *  72  =  33.7°   => -0.374
         *  125 =  26.5°   => -0.294
         *  185 =  18.4°   => -0.204
         * !! This takes into account the reversed picture of the PiCam !!
         * !! Might be needed to be update if lens is moved !!
         * A linear trend gives these coefficients:
         * f(x) = 0.0015604329 x - 0.4902846562
         * R^2 = 0.9999239459 (pretty good!)
         */
        *angle = 0.0015604329 * x - 0.4902846562;
        *distance = 30000.0/size; //magic for now, to be measured later [[[ warning depends on video format FIXME !! ]]]
        return true;
    }
};


#endif//POSITION_MARKER_HPP
