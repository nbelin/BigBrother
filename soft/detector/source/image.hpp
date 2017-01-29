#ifndef IMAGE_H
#define IMAGE_H

#include <cstring>
#include "util.hpp"
#include <iostream>
#include "color.hpp"

class Image {
public:
  Image(void) {}
  Image(unsigned int width, unsigned int height, unsigned char* data) : width(width), height(height), data(data) {}
  unsigned char getValue(unsigned int i, unsigned int j) const;
  void setValue(unsigned int i, unsigned int j, unsigned char val);
  bool isInside(unsigned int i, unsigned int j) const;
  void setData(unsigned char* _data) {
    data = _data;
  }
  void reset(void);
  unsigned int id;
  unsigned int width;
  unsigned int height;
  unsigned char* data;
};

inline unsigned char Image::getValue(unsigned int i, unsigned int j) const {
  return data[i*width + j];
}

inline void Image::setValue(unsigned int i, unsigned int j, unsigned char val) {
  data[i*width + j] = val;
}

inline bool Image::isInside(unsigned int i, unsigned int j) const {
  return (i < height && j < width);
}

inline void Image::reset(void) {
  memset(data, 0, width*height);
}

#include "image_3d.hpp"

#endif //IMAGE_H
