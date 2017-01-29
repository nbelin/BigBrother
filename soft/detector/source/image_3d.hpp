#ifndef IMAGE_3D_HPP
#define IMAGE_3D_HPP

class Image3D {
public:
  Image3D(unsigned int width, unsigned int height, unsigned char* data) : width(width), height(height), data(data) {}
  Image3D(const Image3D& other) : width(other.width), height(other.height), data(other.data) {}
  Color differenceAbsColor(unsigned int i1, unsigned int j1, unsigned int i2, unsigned int j2) const;
  Color getValue(unsigned int i, unsigned int j) const;
  Color getValue(unsigned int index) const;
  void setValue(unsigned int i, unsigned int j, const Color& val);
  unsigned char getH(unsigned int i, unsigned int j) const;
  unsigned char getS(unsigned int i, unsigned int j) const;
  unsigned char getV(unsigned int i, unsigned int j) const;
  void setData(unsigned char* _data) {
    data = _data;
  }
  void reset(void);
  unsigned int id;
  unsigned int width;
  unsigned int height;
  unsigned char* data;
};

inline Color Image3D::differenceAbsColor(unsigned int i1, unsigned int j1, unsigned int i2, unsigned int j2) const {
  unsigned int index1 = (i1 * width + j1) * 3;
  unsigned int index2 = (i2 * width + j2) * 3;
  unsigned char dH = absdiff(data[index1 + 0], data[index2 + 0]);
  unsigned char dS = absdiff(data[index1 + 1], data[index2 + 1]);
  unsigned char dV = absdiff(data[index1 + 2], data[index2 + 2]);
  return Color(dH, dS, dV);
}

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

inline unsigned char Image3D::getH(unsigned int i, unsigned int j) const {
  return data[(i*width + j) * 3];
}

inline unsigned char Image3D::getS(unsigned int i, unsigned int j) const {
  return data[(i*width + j) * 3 + 1];
}

inline unsigned char Image3D::getV(unsigned int i, unsigned int j) const {
  return data[(i*width + j) * 3 + 2];
}

inline void Image3D::reset(void) {
  memset(data, 0, width*height*3);
}

#endif//IMAGE_3D_HPP
