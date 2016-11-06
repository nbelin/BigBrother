#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <vector>
#include "image.hpp"

class Area {
public:
  unsigned int minI;
  unsigned int maxI;
  unsigned int minJ;
  unsigned int maxJ;
  unsigned int size;
  unsigned int width;
  unsigned int height;
  unsigned int count;
  float rank;
  inline bool isInside(unsigned int i, unsigned int j) const {
    return (i >= minI && i<= maxI &&
	    j >= minJ && j <= maxJ);
  }
};

class Rectangle {
public:
  Rectangle(void) {}
  Rectangle(const Color& min, const Color& max);
  bool getArea(const Image3D& image, Image& mask, const Area& searchArea, Area& area);
private:
  Color min;
  Color max;
  unsigned int startIvec;
  unsigned int endIvec;
  unsigned int startJvec;
  unsigned int endJvec;
public:
  bool isRightColor(const Color& val) const;
  bool isPixelRightColor(const Image3D& image, unsigned int i, unsigned int j) const;
  void rateArea(Area& area);
  void expandArea(const Image3D& image, Image& mask, Area& area, unsigned int i, unsigned int j);
};


#endif //RECTANGLE_H
