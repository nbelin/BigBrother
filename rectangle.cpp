#include "rectangle.hpp"
#include "util.hpp"

#include <iostream>
#include <stack>

Rectangle::Rectangle(const Color& min, const Color& max) : min(min), max(max) {}

bool Rectangle::getArea(const Image3D& image, Image& mask, const Area& searchArea, Area& area) {
    std::cout << "color min :" << (int)min.v1 << " " << (int)min.v2 << " " << (int)min.v3 << std::endl;
    std::cout << "color max :" << (int)max.v1 << " " << (int)max.v2 << " " << (int)max.v3 << std::endl;
    std::cout << "search in " << searchArea.minI << " " << searchArea.maxI << std::endl;
    std::cout << "          " << searchArea.minJ << " " << searchArea.maxJ << std::endl;

    unsigned int i = (searchArea.minI + searchArea.maxI) / 2;
    unsigned int j = (searchArea.minJ + searchArea.maxJ) / 2;

    if (! isRightColor(image.getValue(i, j))) {
        return false;
    }

    if (! mask.isInside(i, j)) {
        i = 0;
        j = 0;
    }

    // find approximate top-left corner of the area
    /*while (mask.isInside(i-1, j-1) && isPixelRightColor(image, i-1, j-1)) {
    i--;
    j--;
    }*/

    // make sure we start on the right color
    /*while (searchArea.isInside(i, j) && ! isPixelRightColor(image, i, j)) {
    i++;
    j++;
    }*/

    if (! mask.isInside(i, j)) {
        return false;
    }

    // expand area while updating mask and vectors
    expandArea(image, mask, area, i, j);

    // rate the expanded area
    rateArea(area);

    std::cout << "::" << area.size << " " << area.count << std::endl;
    return area.size > area.count * 2;
}

inline bool Rectangle::isRightColor(const Color& val) const {
    return min <= val && val <= max;
}

bool Rectangle::isPixelRightColor(const Image3D& image, unsigned int i, unsigned int j) const {
    return isRightColor(image.getValue(i, j));
}

void Rectangle::expandArea(const Image3D& image, Image& mask, Area& area, unsigned int i, unsigned int j) {
    //direction is : LEFT, RIGHT, UP, DOWN
    static const int DIRI[] = {-1,  1,  0,  0};
    static const int DIRJ[] = { 0,  0, -1,  1};
    static const Color MAXDIFFCOLOR = Color(30, 200, 200);

    if (isPixelRightColor(image, i, j) == 0) {
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
            if (mask.isInside(ni, nj) && mask.getValue(ni, nj) == 0 && isPixelRightColor(image, ni, nj) && (true || image.differenceAbsColor(i, j, ni, nj) <= MAXDIFFCOLOR)) {
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
    // first verify width/height ratio to cut of the evaluation is not good enough
    unsigned int ratio = area.width / area.height;
    if (ratio < 2 || ratio > 5) {
        area.rank = 0;
        //std::cout << ratio << std::endl;
        //std::cout << area.width << " / " << area.height << std::endl;
        return;
    }

    area.rank = (area.size) / (area.count);
}
