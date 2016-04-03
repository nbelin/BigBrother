#include "marker.hpp"
#include "util.hpp"
#include <iostream>
#include <cstdlib>
#include <algorithm>

Marker::Marker(bool isEnemy, Color color1min, Color color1max, Color color2min, Color color2max, std::vector<bool> binaryCode) : isEnemy(isEnemy), binaryCode(binaryCode) {

    rects[0] = Rectangle(color1min, color1max);
    rects[1] = Rectangle(color2min, color2max);

    for (int i=0; i<2; ++i) {
        masks_vec[i].resize(640 * 480);
        masks[i] = Image(640, 480, masks_vec[i].data());
    }

    //sumLineBinary.resize(500);
}

void Marker::convertPositionMarker2Area(const Image3D& image, const PositionMarker& pm, Area& area, int rectID) const {
    area.minJ = pm.x - pm.size/2;
    area.maxJ = pm.x + pm.size/2;
    int coef;
    if (isEnemy) {
        coef = 1;
    } else {
        coef = -1;
    }
    int offset;
    if (rectID == 0) {
        offset = 0;
    } else {
        offset = pm.size;
    }
    unsigned int approxMiddleI = image.height/2 + coef*offset;
    area.minI = approxMiddleI - 10;
    area.maxI = approxMiddleI + 10;
}

void Marker::convertArea2PositionMarker(const Image3D& image, const Area& area, PositionMarker& pm, int rectID) const {
    pm.x = (area.minI + area.minI) / 2;
    pm.size = area.minI - area.minI;
}

bool Marker::getNextPos(const Image3D& image, PositionMarker &nextPos) {
    for (int i=0; i<2; ++i) {
        memset(masks[i].data, 0, masks[i].width * masks[i].height);
    }

    unsigned int startI = (previousAreas[0].maxI + previousAreas[0].minI)/2;
    unsigned int startJ = (previousAreas[0].maxJ + previousAreas[0].minJ)/2;
    if (startI < image.height && startJ < image.width &&
            detectFromPoint(image, nextPos, startI, startJ)) {
        return true;
    }
    //std::cout << "not found 1st : " << startI << ", " << startJ << std::endl;
    return detectFromZero(image, nextPos);

    if (previousPos.imageID == 0) {
        previousPos.x = image.width/2;
        previousPos.size = image.width/10;
        for (unsigned int i=0; i<2; ++i) {
            convertPositionMarker2Area(image, previousPos, previousAreas[i], i);
        }
    }
    int dID = image.id - previousPos.imageID;
    std::cout << "dID = " << dID << std::endl;
    Area resultArea;
    for (int i=0; i<2; ++i) {
        masks[i].reset();
        unsigned int minJdone = previousAreas[i].minJ;
        unsigned int maxJdone = previousAreas[i].maxJ;
        std::cout << " :) " << i << std::endl;

        while (! rects[i].getArea(image, masks[i], previousAreas[i], resultArea)) {
            std::cout << previousAreas[i].minJ << " , " << previousAreas[i].maxJ << std::endl;
            if (! getOtherArea(image, previousAreas[i], minJdone, maxJdone)) {
                return false;
            }
        }

        std::cout << resultArea.minJ << " ; " << resultArea.maxJ << std::endl;
        std::cout << resultArea.error << " / " << resultArea.size << std::endl;
        for (unsigned int index=0; index<masks[i].width*masks[i].height; ++index) {
            if (masks[i].data[index] == 1) {
                std::cout << index << " \t";
            }
        }
        std::cout << "/end" << std::endl;
        previousAreas[i] = resultArea;
    }
    if (absdiff(previousAreas[0].minJ, previousAreas[1].minJ) < 10 &&
            absdiff(previousAreas[0].maxJ, previousAreas[1].maxJ) < 10) {
        nextPos.imageID = image.id;
        nextPos.x = (previousAreas[0].minJ + previousAreas[0].maxJ) / 2;
        nextPos.size = previousAreas[0].maxJ - previousAreas[0].minJ;
        nextPos.confidence = 1.f;
        nextPos.dx = nextPos.x - previousPos.x;
        nextPos.dsize = nextPos.size - previousPos.size;

        previousPos = nextPos;
        return true;
    }
    return false;
}

bool Marker::getOtherArea(const Image3D& image, Area &area, unsigned int& minJdone, unsigned int& maxJdone) {
    unsigned int dmin = previousPos.x - minJdone;
    unsigned int dmax = maxJdone - previousPos.x;
    if (dmin < dmax && minJdone > 10) {
        if (minJdone < previousPos.size)
            area.minJ = 0;
        else
            area.minJ = minJdone - previousPos.size;

        area.maxJ = minJdone;
        minJdone = area.minJ;
        return true;
    } else if (maxJdone < image.width - 10) {
        if (maxJdone < image.width - previousPos.size -1)
            area.maxJ = image.width-1;
        else
            area.maxJ = maxJdone + previousPos.size;

        area.minJ = maxJdone;
        maxJdone = area.maxJ;
        return true;
    } else {
        return false;
    }
}

bool Marker::detectFromPoint(const Image3D &image, PositionMarker &nextPos, unsigned int startI, unsigned int startJ) {
    if (masks[0].getValue(startI, startJ) == 0 && rects[0].isPixelRightColor(image, startI, startJ)) {

        rects[0].expandArea(image, masks[0], previousAreas[0], startI, startJ);
        if (previousAreas[0].size < MIN_AREA_SIZE || previousAreas[0].size > MAX_AREA_SIZE) {
            return false;
        }
        //reset second masks ... to FIX for perf
        memset(masks[1].data, 0, masks[1].width * masks[1].height);

        rects[0].rateArea(previousAreas[0]);
        if (previousAreas[0].rank < 1) {
            return false;
        }

        const unsigned int length = previousAreas[0].maxJ - previousAreas[0].minJ;
        const unsigned int nj = previousAreas[0].minJ + length/2;
        const unsigned int iMax = std::max(startI + length + 20, image.height);

        for (unsigned int ni = startI; ni < iMax; ++ni) {
            if (rects[1].isPixelRightColor(image, ni, nj)) {

                bool toDebug = false;
                rects[1].expandArea(image, masks[1], previousAreas[1], ni, nj);
                if (previousAreas[1].size < MIN_AREA_SIZE || previousAreas[1].size > MAX_AREA_SIZE) {
                    return false;
                }

                rects[1].rateArea(previousAreas[1]);
                if (previousAreas[1].rank < 1) {
                    std::cout << previousAreas[1].rank << std::endl;
                    return false;
                }

                // std::cout << previousAreas[1].size << "\t";
                // std::cout << previousAreas[1].minJ << "\t";
                // std::cout << previousAreas[1].maxJ << " ? " << std::endl;
                const unsigned int diffLeft = absdiff(previousAreas[0].minJ, previousAreas[1].minJ);
                const unsigned int diffRight = absdiff(previousAreas[0].maxJ, previousAreas[1].maxJ);
                const unsigned int markerHeight = previousAreas[1].maxI - previousAreas[0].minI;
                const unsigned int markerWidth = previousAreas[0].maxJ - previousAreas[0].minJ;
                const unsigned int diffHW = absdiff(markerHeight, markerWidth);
                // std::cout << diffLeft << " " << diffRight << " " << diffHW << std::endl;
                if (diffLeft < 30 && diffRight < 30 && diffHW < 40) {
                    nextPos.imageID = image.id;
                    nextPos.x = nj;
                    nextPos.size = length;
                    nextPos.confidence = previousAreas[0].rank;
                    nextPos.dx = 0;
                    nextPos.dsize = 0;
                    return true;
                }
            }
        }
    }
    return false;
}

bool Marker::detectFromZero(const Image3D& image, PositionMarker& nextPos) {
    for (unsigned int i=50; i<image.height-50; i+=2) {
        for (unsigned int j=10; j<image.width-10; j+=4) {
            if (detectFromPoint(image, nextPos, i, j) == true) {
                return true;
            }
        }
    }
    return false;
}

/*
  Binary code describes the edges of the marker.
  True means rising edge (to white).
  False means falling edge (to black).
  Color squares take part of the edges.
*/
bool Marker::checkBinary(const Image3D& image, PositionMarker& nextPos) {
    const int THRESH_WHITE_UNIT = 160;
    const int THRESH_BLACK_UNIT = 110;
    const int SIZE_HALF = 5;
    const int SIZE = 2 * SIZE_HALF + 1;
    const int THRESH_WHITE = THRESH_WHITE_UNIT * SIZE;
    const int THRESH_BLACK = THRESH_BLACK_UNIT * SIZE;
    const unsigned int startJ = (previousAreas[0].minJ + previousAreas[0].maxJ) / 2 - SIZE_HALF;
    const unsigned int endJ = (previousAreas[0].minJ + previousAreas[0].maxJ) / 2 + SIZE_HALF;
    const unsigned int startI = (previousAreas[0].minI + previousAreas[0].maxI)/2;
    const unsigned int endI = (previousAreas[1].minI + previousAreas[1].maxI)/2;
    // std::cout << "checkBinary" << std::endl;
    // std::cout << THRESH_BLACK << std::endl;
    // std::cout << THRESH_WHITE << std::endl;

    size_t index = 0;
    bool previousComp, comp;
    for (unsigned int i=startI; i<=endI; ++i) {
        //sumLineBinary[i - startI] = 0;
        int sum = 0;
        for (unsigned int j=startJ; j<=endJ; ++j) {
            //sumLineBinary[i - startI] += image.getV(i, j);
            sum += image.getV(i, j);
        }
        //std::cout << sum << std::endl;
        if (previousComp) { //was white, now want black
            comp = (sum > THRESH_BLACK);
        } else { //was black, now want white
            comp = (sum > THRESH_WHITE);
        }
        //std::cout << comp << std::endl;
        if ((index == 0 || previousComp != comp) &&
                binaryCode[index++] != comp) {
            // std::cout << previousComp << " " << comp << std::endl;
            // std::cout << index << " " << binaryCode[index] << std::endl;
            return false;
        }
        previousComp = comp;
        //std::cout << i << " " << sumLineBinary[i - startI] << std::endl;
    }
    // std::cout << index << std::endl;
    // std::cout << binaryCode.size() << std::endl;
    return index == binaryCode.size();
}
