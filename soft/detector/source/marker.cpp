#include "marker.hpp"
#include "util.hpp"
#include <iostream>
#include <cstdlib>
#include <algorithm>

Marker::Marker(const Image3D& firstImage, bool isEnemy, const Rectangle &rect1, const Rectangle &rect2, const Rectangle &rect3) : isEnemy(isEnemy) {

    rects[0] = rect1;
    rects[1] = rect2;
    rects[2] = rect3;
    previousPos.imageID = 0;
    previousPos.x = 0;
    previousPos.size = 0;
    previousPos.minI = 0;
    previousPos.maxI = 0;
    previousPos.confidence = 0.f;
    previousPos.dx = 0;
    previousPos.dsize = 0;

    for (int i=0; i<3; ++i) {
        masks_vec[i].resize(firstImage.width * firstImage.height);
        masks[i] = Image(firstImage.width, firstImage.height, masks_vec[i].data());
    }
}

Marker::Marker(const Image3D& firstImage, bool isEnemy, const ColorSet& rect1, const ColorSet& rect2, const ColorSet& rect3)
    : Marker(firstImage, isEnemy, Rectangle(rect1), Rectangle(rect2), Rectangle(rect3)) {
}

bool Marker::getNextPos(const Image3D& image, PositionMarker &nextPos) {
    //std::cout << "getNextPos !" << std::endl;
    nextPos.reset();
    for (int i=0; i<3; ++i) {
        masks[i].reset();
    }

    if (isMarkerFound(previousPos)) {
        unsigned int startI = (previousAreas[0].maxI + previousAreas[0].minI)/2;
        unsigned int startJ = (previousAreas[0].maxJ + previousAreas[0].minJ)/2;
        detectFromPoint(image, nextPos, startI, startJ);
        //detectFromPrevious(image, nextPos);
        if (isMarkerFound(nextPos)) {
            previousPos = nextPos;
            return true;
        }
        std::cout << "not found 1st : " << startI << ", " << startJ << std::endl;
        for (int di=-1; di<2; ++di) {
            for (int dj=-1; dj<2; ++dj) {
                detectFromPoint(image, nextPos, startI+di, startJ+dj);
                if (isMarkerFound(nextPos)) {
                    previousPos = nextPos;
                    return true;
                }
            }
        }
        std::cout << "not found even after square" << std::endl;
    }

    //std::cout << "previous marker not found ?" << std::endl;
    previousPos.reset();

    return detectFromZero(image, nextPos);
}

bool Marker::detectFromPoint(const Image3D &image, PositionMarker &nextPos, unsigned int startI, unsigned int startJ) {
    if (! masks[0].isInside(startI, startJ)) {
        return false;
    }

    if (masks[0].getValue(startI, startJ) == 0 && rects[0].isPixelRightColor(image, startI, startJ)) {

        rects[0].expandArea(image, masks[0], previousAreas[0], startI, startJ);

        rects[0].rateArea(previousAreas[0]);
        //previousAreas[0].display();
        if (previousAreas[0].rank < 0.6) {
            return false;
        }

        //std::cout << "OK :)\n";

        //reset second masks ... to FIX for perf
        //masks[1].reset();

        const unsigned int length = previousAreas[0].maxJ - previousAreas[0].minJ;
        const unsigned int nj = previousAreas[0].minJ + length/2;
        const unsigned int iMax = std::max(previousAreas[0].maxI + 10, image.height);

        for (unsigned int ni = startI; ni < iMax; ++ni) {
            if (rects[1].isPixelRightColor(image, ni, nj)) {

                rects[1].expandArea(image, masks[1], previousAreas[1], ni, nj);

                rects[1].rateArea(previousAreas[1]);
                //previousAreas[1].display();
                if (previousAreas[1].rank < 0.6) {
                    return false;
                }

                const unsigned int iMax = std::max(previousAreas[1].maxI + 10, image.height);

                for (unsigned int nni = startI; nni < iMax; ++nni) {
                    if (rects[2].isPixelRightColor(image, nni, nj)) {

                        rects[2].expandArea(image, masks[2], previousAreas[2], nni, nj);

                        rects[1].rateArea(previousAreas[2]);
                        //previousAreas[2].display();
                        if (previousAreas[2].rank < 0.6) {
                            return false;
                        }

                        ratePositionMarker(image, nextPos);
                        if (isMarkerFound(nextPos)) {
                            previousPos = nextPos;
                            return true;
                        }
                        std::cout << "oh almost :(" << std::endl;
                        nextPos.display();
                    }
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

bool Marker::detectFromPrevious(const Image3D& image, PositionMarker& nextPost) {
    for (size_t id=0; id<3; ++id) {
        const unsigned int startI = (previousAreas[id].maxI + previousAreas[id].minI)/2;
        const unsigned int startJ = (previousAreas[id].maxJ + previousAreas[id].minJ)/2;
        rects[id].expandArea(image, masks[id], previousAreas[id], startI, startJ);
        rects[id].rateArea(previousAreas[id]);
        if (previousAreas[id].rank < 0.5) {
            return false;
        }
    }
    ratePositionMarker(image, nextPost);
    if (isMarkerFound(nextPost)) {
        previousPos = nextPost;
        return true;
    }
    previousPos.reset();
    return false;
}

void Marker::ratePositionMarker(const Image3D& image, PositionMarker& pm) {
    // compute mean J interval for global marker
    unsigned int meanMinJ = 0;
    for (size_t id=0; id<3; ++id) {
        meanMinJ += previousAreas[id].minJ;
    }
    meanMinJ /= 3;
    unsigned int meanMaxJ = 0;
    for (size_t id=0; id<3; ++id) {
        meanMaxJ += previousAreas[id].maxJ;
    }
    meanMaxJ /= 3;

    // feed most properties for PositionMarker (except confidence)
    pm.imageID = image.id;
    pm.x = ( meanMinJ + meanMaxJ ) / 2;
    pm.size = meanMaxJ - meanMinJ;
    pm.minI = previousAreas[0].minI;
    pm.maxI = previousAreas[2].maxI;
    if (isMarkerFound(previousPos)) {
        pm.dx = (int)previousPos.x - (int)pm.x;
        pm.dsize = (int)previousPos.size - (int)pm.size;
    }

    // make sure each detected area is close from each other
    unsigned int distBetweenAreas = 0;
    for (size_t id=1; id<3; ++id) {
        distBetweenAreas += absdiff(previousAreas[id-1].maxI, previousAreas[id].minI);
    }

    // compute difference with the mean
    unsigned int distMinJ = 0;
    for (size_t id=0; id<3; ++id) {
        distMinJ += absdiff(meanMinJ, previousAreas[id].minJ);
    }
    unsigned int distMaxJ = 0;
    for (size_t id=0; id<3; ++id) {
        distMaxJ += absdiff(meanMaxJ, previousAreas[id].maxJ);
    }

    // compute difference between (mean)width and height
    unsigned int height = pm.maxI - pm.minI;
    unsigned int diffWidthHeight = absdiff(pm.size, height);

    // compute distance from middle of image (depending enemy or not)
    unsigned int distFromMiddle;
    unsigned int imageMiddleHeight = image.height / 2;
    if (isEnemy) {
        distFromMiddle = absdiff(imageMiddleHeight, pm.maxI);
    } else {
        distFromMiddle = absdiff(imageMiddleHeight, pm.minI);
    }

    float confidence_distFromMiddle = 0.05f * (float) distFromMiddle / (float) pm.size;
    float confidence_diffWidthHeight = 0.5f * (float) diffWidthHeight / (float) pm.size;
    //std::cout << "conf_distFromMiddle=" << confidence_distFromMiddle << std::endl;
    //std::cout << "conf_diffWidthHeight=" << confidence_diffWidthHeight << std::endl;
    pm.confidence = 1.f - confidence_distFromMiddle - confidence_diffWidthHeight;
}

bool Marker::isMarkerFound(const PositionMarker& pm) {
    return pm.confidence > 0.5;
}

