#include "rectangle.hpp"
#include "util.hpp"

#include <iostream>
#include <stack>

Rectangle::Rectangle(void) : color_set(Color(0,0,0), 0, Color(0,0,0), Color(0,0,0)) {}
Rectangle::Rectangle(const ColorSet& color_set) : color_set(color_set) {}

bool Rectangle::isPixelRightColor(int color_mode, const Image3D& image, unsigned int i, unsigned int j) const {
    return color_set.contains(color_mode, image.getValue(i, j));
}

void Rectangle::expandArea(int color_mode, const Image3D& image, Image& mask, Area& area, unsigned int i, unsigned int j) {
    //direction is : LEFT, RIGHT, UP, DOWN
    static const int DIRI[] = {-1,  1,  0,  0};
    static const int DIRJ[] = { 0,  0, -1,  1};
    static const Color MAXDIFFCOLOR = Color(4, 4, 4);

    if (isPixelRightColor(color_mode, image, i, j) == false) {
        area.size = 0;
        return;
    }

    area.minI = mask.height;
    area.maxI = 0;
    area.minJ = mask.width;
    area.maxJ = 0;
    area.count = 0;

    std::stack<unsigned int> stackI;
    std::stack<unsigned int> stackJ;
    stackJ.push(j);
    stackI.push(i);
    mask.setValue(i, j, 1);
    while (! stackI.empty()) {
        i = stackI.top();
        stackI.pop();
        j = stackJ.top();
        stackJ.pop();
        area.count ++;

        // update min and max
        if (j < area.minJ)
            area.minJ = j;
        if (j > area.maxJ)
            area.maxJ = j;
        if (i < area.minI)
            area.minI = i;
        if (i > area.maxI)
            area.maxI = i;

        // for each neighbors (left, right, up, down)
        for (unsigned int dir=0; dir<4; ++dir) {
            int ni = i+DIRI[dir];
            int nj = j+DIRJ[dir];

            //if not visited yet AND is right color AND diff color is not too important
            if ((mask.isInside(ni, nj) && mask.getValue(ni, nj) == 0) &&
                    (isPixelRightColor(color_mode, image, ni, nj)|| image.differenceAbsColor(i, j, ni, nj) <= MAXDIFFCOLOR)) {
                stackI.push(ni);
                stackJ.push(nj);
                mask.setValue(ni, nj, 1);
            }
        }
    }

    area.height = area.maxI - area.minI + 1;
    area.width = area.maxJ - area.minJ + 1;
    area.size = area.height * area.width;
}

void Rectangle::rateArea(Area& area) {
    // first, minimal check on area width
    if (area.height < MIN_AREA_HEIGHT || area.width < MIN_AREA_WIDTH || area.width > MAX_AREA_WIDTH) {
        area.rank = 0;
        return;
    }

    // second, verify width/height ratio to cut of the evaluation is not good enough
    /*
     *          8 cm
     * +----------------------+
     * |   @@@@@@@@@@@@@@@@@  |
     * |  @@@@@@@@@@@@@@@@@@  |  2.5 cm    [ !! consider 5 cm for new video format !! ]
     * |    @@@@@@@@@@@@@@    |
     * +----------------------+
     *
     * The full height (2.5 cm) is often fully seen.
     * The full width (8 cm) is often NOT fully seen.
     * We want that :
     *  - the width is at least once the height
     *  - the width is at most 8 times the height
     * Further checks will be done at higher level
     */
    float ratio = area.width / area.height;
    //if (ratio < 1 || ratio > 7) {
    if (ratio < 0.8 || ratio > 4) {
        area.rank = 0;
        //std::cout << ratio << std::endl;
        //std::cout << area.width << " / " << area.height << std::endl;
        return;
    }

    area.rank = ( ( (float) (area.count) ) / ( (float) (area.size) ) );
}
