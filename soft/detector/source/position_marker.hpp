#ifndef POSITION_MARKER_HPP
#define POSITION_MARKER_HPP

class PositionMarker {
public:
    PositionMarker(int pmID = 0) : pmID(pmID), imageID(0), x(0), size(0), confidence(0), dx(0), dsize(0) {}
    unsigned int pmID;
    unsigned int imageID;
    unsigned int x;
    unsigned int size;
    unsigned int minI;
    unsigned int maxI;
    float confidence;
    int dx;
    int dsize;
    void display() {
        std::cout << "pmID=" << pmID << " imID=" << imageID << " \tx=" << x << " \tsize=" << size << std::endl;
        std::cout << "minI=" << minI << " \tmaxI=" << maxI << std::endl;
        std::cout << "confidence=" << confidence << " \tdx=" << dx << " \tdsize=" << dsize << std::endl;
    }
    void reset() {
        imageID = 0;
        x = 0;
        size = 0;
        minI = 0;
        maxI = 0;
        confidence = 0;
        dx = 0;
        dsize = 0;
    }
    bool hasBeenFound() {
        return confidence > 0;
    }
    bool toWorld(float * angle, int * distance) {
        if (angle == nullptr || distance == nullptr) {
            return false;
        }
        /*
         * - angle from -1 (90° to left) to 1 (90° to right)
         * - distance in mm
         * Some pixels have been associated to angles:
         *  632 = -45°     => -0.5
         *  556 = -33.7°   => -0.374
         *  445 = -18.4°   => -0.204
         *  414 = -14°     => -0.156
         *  316 =   0°     =>  0
         *  72  =  33.7°   =>  0.374
         *  125 =  26.5°   =>  0.294
         *  185 =  18.4°   =>  0.204
         * !! This takes into account the reversed picture of the PiCam !!
         * !! Might be needed to be update if lens is moved !!
         * A linear trend gives these coefficients:
         * f(x) = -0.0015604329 x + 0.4902846562
         * R^2 = 0.9999239459 (pretty good!)
         */
        *angle = -0.0015604329 * x + 0.4902846562;
        *distance = 50000.0/size; //magic for now, to be measured later
        return true;
    }
};


#endif//POSITION_MARKER_HPP
