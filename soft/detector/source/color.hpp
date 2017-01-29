#ifndef COLOR_HPP
#define COLOR_HPP

class Color {
public:
  Color(void) {}
  Color(unsigned char v1, unsigned char v2, unsigned char v3) : v1(v1), v2(v2), v3(v3) {}
  bool operator<=(const Color& other) const {
    return v1 <= other.v1 && v2 <= other.v2 && v3 <= other.v3;
  }
  unsigned int luv_square_dist(const Color& other) const {
      int diff_u = (int)v2 - (int)other.v2;
      unsigned int square_diff_u = diff_u * diff_u;
      int diff_v = (int)v3 - (int)other.v3;
      unsigned int square_diff_v = diff_v * diff_v;
      return square_diff_u + square_diff_v;
  }

  unsigned char v1;
  unsigned char v2;
  unsigned char v3;
};

#endif//COLOR_HPP
