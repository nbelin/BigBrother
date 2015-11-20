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
  rateArea(image, mask, area, i, j);

  std::cout << "::" << area.size << " " << area.error << std::endl;
  return area.size > area.error * 2;
}

inline bool Rectangle::isRightColor(const Color& val) const {
  return min <= val && val <= max;
}

inline bool Rectangle::isPixelRightColor(const Image3D& image, unsigned int i, unsigned int j) const {
  return isRightColor(image.getValue(i, j));
}

void Rectangle::expandArea(const Image3D& image, Image& mask, Area& area, unsigned int i, unsigned int j) {
  //direction is : LEFT, RIGHT, UP, DOWN
  static const int DIRI[] = {-1,  1,  0,  0};
  static const int DIRJ[] = { 0,  0, -1,  1};

  minI.resize(mask.width);
  std::fill(minI.begin(), minI.end(), -1);
  maxI.resize(mask.width);
  std::fill(maxI.begin(), maxI.end(), -1);
  minJ.resize(mask.height);
  std::fill(minJ.begin(), minJ.end(), -1);
  maxJ.resize(mask.height);
  std::fill(maxJ.begin(), maxJ.end(), -1);

  startIvec = mask.height;
  endIvec = 0;
  startJvec = mask.width;
  endJvec = 0;


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

    std::cout << "ij = " << i << ", " << j << std::endl;

    // update vectors
    if (minI[j] < 0) { //this column has not been seen yet
      minI[j] = i;
      maxI[j] = i;
      if (j < startJvec)
	startJvec = j;
      if (j > endJvec)
	endJvec = j;
    }
    if (minJ[i] < 0) { //this row has not been seen yet
      minJ[i] = j;
      maxI[i] = j;
      if (i < startIvec)
	startIvec = i;
      if (i > endIvec)
	endIvec = i;
    }
    if (i < minI[j])
      minI[j] = i;
    if (i > maxI[j])
      maxI[j] = i;
    if (j < minJ[i])
      minJ[i] = j;
    if (j > maxJ[i])
      maxI[i] = j;

    // for each neighbors (left, right, up, down)
    for (unsigned int dir=0; dir<4; ++dir) {
      int ni = i+DIRI[dir];
      int nj = j+DIRJ[dir];
      //if not visited yet AND is right color
      if (mask.isInside(ni, nj) && mask.getValue(ni, nj) == 0 && isPixelRightColor(image, ni, nj)) {
	stackI.push(ni);
	stackJ.push(nj);
	mask.setValue(ni, nj, 1);
      }
    }
  }

  area.minI = startIvec;
  area.maxI = endIvec;
  area.minJ = startJvec;
  area.maxJ = endJvec;
  area.size = (endIvec - startIvec + 1) * (endJvec - startJvec + 1);
}

void Rectangle::rateArea(const Image3D& image, Image& mask, Area& area, unsigned int i, unsigned int j) {
  // extract square quality from vectors
  unsigned int minIsum = 0;
  unsigned int maxIsum = 0;
  unsigned int minJsum = 0;
  unsigned int maxJsum = 0;
  area.error = 0;
  for (i=startIvec; i<=endIvec; ++i) {
    minJsum += minJ[i];
    maxJsum += maxJ[i];
  }
  unsigned int minJmean = minJsum / (1 + endIvec - startIvec);
  unsigned int maxJmean = maxJsum / (1 + endIvec - startIvec);
  for (i=startIvec; i<=endIvec; ++i) {
    area.error += absdiff(minJmean, minJ[i]);
    area.error += absdiff(maxJmean, maxJ[i]);
  }

  for (j=startJvec; j<=endJvec; ++j) {
    minIsum += minI[j];
    maxIsum += maxI[j];
  }
  unsigned int minImean = minIsum / (1 + endJvec - startJvec);
  unsigned int maxImean = maxIsum / (1 + endJvec - startJvec);
  for (j=startJvec; j<=endJvec; ++j) {
    if (minI[j] >= 0) {
      area.error += absdiff(minImean, minI[j]);
      area.error += absdiff(maxImean, maxI[j]);
    }
  }
}
