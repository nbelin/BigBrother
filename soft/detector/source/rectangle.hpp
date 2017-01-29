#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include <vector>
#include "image.hpp"
#include "area.hpp"

class Rectangle {
public:
  Rectangle(void) {}
  Rectangle(const Color& average, unsigned int radius);
private:
  Color average;
  unsigned int radius;
  unsigned int startIvec;
  unsigned int endIvec;
  unsigned int startJvec;
  unsigned int endJvec;
  static const unsigned int MIN_AREA_WIDTH = 10;
  static const unsigned int MAX_AREA_WIDTH = 1000;
public:
  bool isRightColor(const Color& val) const;
  bool isPixelRightColor(const Image3D& image, unsigned int i, unsigned int j) const;
  void rateArea(Area& area);
  void expandArea(const Image3D& image, Image& mask, Area& area, unsigned int i, unsigned int j);
};


#endif //RECTANGLE_HPP
