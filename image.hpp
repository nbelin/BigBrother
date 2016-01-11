#ifndef IMAGE_H
#define IMAGE_H

#include <cstring>

class Color {
public:
  Color(void) {}
  Color(unsigned char v1, unsigned char v2, unsigned char v3) : v1(v1), v2(v2), v3(v3) {}
  bool operator<=(const Color& other) const {
    return v1 <= other.v1 && v2 <= other.v2 && v3 <= other.v3;
  }
  unsigned char v1;
  unsigned char v2;
  unsigned char v3;
};

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
  return (i >= 0 && i < height && j >= 0 && j < width);
}

inline void Image::reset(void) {
  memset(data, 0, width*height);
}

class Image3D {
public:
  Image3D(unsigned int width, unsigned int height, unsigned char* data) : width(width), height(height), data(data) {}
  Color getValue(unsigned int i, unsigned int j) const;
  Color getValue(unsigned int index) const;
  void setValue(unsigned int i, unsigned int j, const Color& val);
  void setData(unsigned char* _data) {
    data = _data;
  }
  void reset(void);
  unsigned int id;
  unsigned int width;
  unsigned int height;
  unsigned char* data;
};

inline Color Image3D::getValue(unsigned int i, unsigned int j) const {
  return getValue((i*width + j) * 3);
}

inline Color Image3D::getValue(unsigned int index) const {
  return Color(data[index], data[index+1], data[index+2]);
}

inline void Image3D::setValue(unsigned int i, unsigned int j, const Color& val) {
  unsigned int const index = (i*width + j) * 3;
  data[index] = val.v1;
  data[index+1] = val.v2;
  data[index+2] = val.v3;
}

inline void Image3D::reset(void) {
  memset(data, 0, width*height*3);
}

#endif //IMAGE_H
