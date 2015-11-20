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
  Image(int width, int height, unsigned char* data) : width(width), height(height), data(data) {}
  unsigned char getValue(int i, int j) const;
  void setValue(int i, int j, unsigned char val);
  bool isInside(int i, int j) const;
  void setData(unsigned char* _data) {
    data = _data;
  }
  void reset(void);
  int id;
  int width;
  int height;
  unsigned char* data;
};

inline unsigned char Image::getValue(int i, int j) const {
  return data[i*width + j];
}

inline void Image::setValue(int i, int j, unsigned char val) {
  data[i*width + j] = val;
}

inline bool Image::isInside(int i, int j) const {
  return (i >= 0 && i < height && j >= 0 && j < width);
}

inline void Image::reset(void) {
  memset(data, 0, width*height);
}

class Image3D {
public:
  Image3D(int width, int height, unsigned char* data) : width(width), height(height), data(data) {}
  Color getValue(int i, int j) const;
  void setValue(int i, int j, const Color& val);
  void setData(unsigned char* _data) {
    data = _data;
  }
  void reset(void);
  int id;
  int width;
  int height;
  unsigned char* data;
};

inline Color Image3D::getValue(int i, int j) const {
  int const index = (i*width + j) * 3;
  return Color(data[index], data[index+1], data[index+2]);
}

inline void Image3D::setValue(int i, int j, const Color& val) {
  int const index = (i*width + j) * 3;
  data[index] = val.v1;
  data[index+1] = val.v2;
  data[index+2] = val.v3;
}

inline void Image3D::reset(void) {
  memset(data, 0, width*height*3);
}

#endif //IMAGE_H
