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
  float rank; // 0 to 1
  inline bool isInside(unsigned int i, unsigned int j) const {
    return (i >= minI && i <= maxI &&
	    j >= minJ && j <= maxJ);
  }
  void display() {
      std::cout << "(" << minI << ", " << minJ << ") \t -> (" << maxI << ", " << maxJ << ")" << std::endl;
      std::cout << "size=" << size << " \tw=" << width << " \th=" << height << std::endl;
      std::cout << "count=" << count << " \trank=" << rank << std::endl;
  }
};

class Rectangle {
public:
  Rectangle(void) {}
  Rectangle(const Color& average, unsigned int radius);
  bool getArea(const Image3D& image, Image& mask, const Area& searchArea, Area& area);
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


#endif //RECTANGLE_H
