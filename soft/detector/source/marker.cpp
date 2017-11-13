#include "marker.hpp"
#include "util.hpp"
#include <iostream>
#include <cstdlib>
#include <algorithm>

Marker::Marker(const Image3D& firstImage, bool isEnemy, const Rectangle &rect1, const Rectangle &rect2, const Rectangle &rect3) :
    isEnemy(! isEnemy) {
    ///// reversed picture!
    rects[0] = rect3;
    rects[1] = rect2;
    rects[2] = rect1;
    previousPos.reset();

    for (int i=0; i<3; ++i) {
        masks_vec[i].resize(firstImage.width * firstImage.height);
        masks[i] = Image(firstImage.width, firstImage.height, masks_vec[i].data());
    }
}

Marker::Marker(const Image3D& firstImage, bool isEnemy, const ColorSet& rect1, const ColorSet& rect2, const ColorSet& rect3)
    : Marker(firstImage, isEnemy, Rectangle(rect1), Rectangle(rect2), Rectangle(rect3)) {
}

bool Marker::getNextPos(int color_mode, const Image3D& image, PositionMarker &nextPos) {
    this->colorMode = color_mode;
    //std::cout << "getNextPos !\n";
    nextPos.reset();
    for (int i=0; i<3; ++i) {
        masks[i].reset();
    }

    if (isMarkerFound(previousPos)) {
        unsigned int startI = (previousAreas[0].maxI + previousAreas[0].minI)/2 + previousPos.dx;
        unsigned int startJ = (previousAreas[0].maxJ + previousAreas[0].minJ)/2 + previousPos.dx;
        if (detectFromPoint(image, nextPos, startI, startJ)) {
            previousPos = nextPos;
            return true;
        }
        //std::cout << "not found 1st : " << startI << ", " << startJ << "\n";
        for (int di=-1; di<2; ++di) {
            for (int dj=-1; dj<2; ++dj) {
                if (detectFromPoint(image, nextPos, startI+di, startJ+dj)) {
                    previousPos = nextPos;
                    return true;
                }
            }
        }
        //std::cout << "not found even after square\n";
    }

    //std::cout << "previous marker not found ?\n";
    previousPos.reset();

    return detectFromZero(image, nextPos);
}

bool Marker::detectFromPoint(const Image3D &image, PositionMarker &nextPos, unsigned int startI, unsigned int startJ) {
    if (! masks[0].isInside(startI, startJ)) {
        return false;
    }

    if (masks[0].getValue(startI, startJ) == 0 && rects[0].isPixelRightColor(colorMode, image, startI, startJ)) {

        rects[0].expandArea(colorMode, image, masks[0], previousAreas[0], startI, startJ);

        rects[0].rateArea(previousAreas[0]);
        //previousAreas[0].display();
        if (previousAreas[0].rank < 0.5) {
            return false;
        }

        //std::cout << "OK :)\n";

        //reset second masks ... to FIX for perf
        //masks[1].reset();

        const unsigned int length = previousAreas[0].maxJ - previousAreas[0].minJ;
        const unsigned int nj = previousAreas[0].minJ + length/2;
        const unsigned int iMax = std::min(previousAreas[0].maxI + 10, image.height);

        for (unsigned int ni = startI; ni < iMax; ++ni) {
            if (rects[1].isPixelRightColor(colorMode, image, ni, nj)) {

                rects[1].expandArea(colorMode, image, masks[1], previousAreas[1], ni, nj);

                rects[1].rateArea(previousAreas[1]);
                //previousAreas[1].display();
                if (previousAreas[1].rank < 0.5) {
                    return false;
                }

                const unsigned int iMax = std::min(previousAreas[1].maxI + 10, image.height);

                for (unsigned int nni = startI; nni < iMax; ++nni) {
                    if (rects[2].isPixelRightColor(colorMode, image, nni, nj)) {

                        rects[2].expandArea(colorMode, image, masks[2], previousAreas[2], nni, nj);

                        rects[1].rateArea(previousAreas[2]);
                        //previousAreas[2].display();
                        if (previousAreas[2].rank < 0.5) {
                            return false;
                        }

                        ratePositionMarker(image, nextPos);
                        if (isMarkerFound(nextPos)) {
                            previousPos = nextPos;
                            return true;
                        }
                        //std::cout << "oh almost :(\n";
                        //nextPos.display();
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
        rects[id].expandArea(colorMode, image, masks[id], previousAreas[id], startI, startJ);
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
    pm.minI = previousAreas[0].minI;
    pm.maxI = previousAreas[2].maxI;
    pm.size = pm.maxI - pm.minI; // (height)
    if (isMarkerFound(previousPos)) {
        pm.dx = (int)pm.x - (int)previousPos.x;
        pm.dsize = (int)pm.size - (int)previousPos.size;
    }

    // let's verify all areas have similar heights
    unsigned int minAreaHeight = image.height;
    unsigned int maxAreaHeight = 0;
    for (size_t id=0; id<3; ++id) {
        const unsigned int h = previousAreas[id].height;
        if (h < minAreaHeight) {
            minAreaHeight = h;
        }
        if (h > maxAreaHeight) {
            maxAreaHeight = h;
        }
    }
    const unsigned int diffAreaHeight = maxAreaHeight - minAreaHeight;
    const float ratioAreaHeight = (float) maxAreaHeight / minAreaHeight;
    //if (diffAreaHeight > 10 && ratioAreaHeight > 2.f) {
    if (diffAreaHeight > 15 && ratioAreaHeight > 4.f) { //// [[ new video format !! ]]
        std::cout << "ratio area height too high: " << ratioAreaHeight << "\n";
        std::cout << "diff area height too high: " << diffAreaHeight << "\n";
        pm.confidence = 0.f;
        return;
    }

    // make sure each detected area is close from each other
    for (size_t id=1; id<3; ++id) {
        int distBetweenAreas = previousAreas[id-1].maxI - previousAreas[id].minI;
        if (distBetweenAreas < -3) { // allow for small overlap
            std::cout << "distBetweenAreas negative!! : " << distBetweenAreas << "\n";
            pm.confidence = 0.f;
            return;
        }
        //if (distBetweenAreas > 10) {
        if (distBetweenAreas > 15) {
            std::cout << "distBetweenAreas too large: " << distBetweenAreas << "\n";
            pm.confidence = 0.f;
            return;
        }
    }

    // width cannot be greater than height   [[[ new video format !! ]]]
    // width could be a bit smaller than height if not fully seen
    unsigned int width = meanMaxJ - meanMinJ;
    float ratio_height_width = (float) pm.size / width;
    if (ratio_height_width < 0.7f || ratio_height_width > 6.f) {
        std::cout << "ratio not good: " << ratio_height_width << "\n";
        pm.confidence = 0.f;
        return;
    }

    // compute distance from middle of image (depending enemy or not)
    unsigned int distFromMiddle;
    unsigned int imageMiddleHeight = image.height / 2;
    if (isEnemy) {
        distFromMiddle = absdiff(imageMiddleHeight, pm.maxI);
    } else {
        distFromMiddle = absdiff(imageMiddleHeight, pm.minI);
    }
    if (distFromMiddle > 40) { //// [[[ new video format !! ]]]
        std::cout << "distFromMiddle too large: " << distFromMiddle << "\n";
        pm.confidence = 0.f;
        return;
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

    float confidence_distJ = 0.5f * (float) (distMinJ + distMaxJ) / (float) pm.size;
    std::cout << "OK :) conf_distJ=" << confidence_distJ << "\n";
    //std::cout << "conf_diffWidthHeight=" << confidence_diffWidthHeight << "\n;
    pm.confidence = 1.f - confidence_distJ;
    //previousAreas[0].display();
    //previousAreas[1].display();
    //previousAreas[2].display();
}

bool Marker::isMarkerFound(const PositionMarker& pm) {
    return pm.confidence > 0.5;
}

