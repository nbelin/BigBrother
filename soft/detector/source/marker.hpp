#ifndef MARKER_HPP
#define MARKER_HPP

#include "image.hpp"
#include "rectangle.hpp"
#include <vector>

class PositionMarker {
public:
  PositionMarker(int pmID) : pmID(pmID), imageID(0), x(0), size(0), confidence(0), dx(0), dsize(0) {}
  unsigned int pmID;
  unsigned int imageID;
  unsigned int x;
  unsigned int size;
  unsigned int minI;
  unsigned int maxI;
  float confidence;
  int dx;
  int dsize;
  void display() {
      std::cout << "id=" << imageID << " \tx=" << x << " \tsize=" << size << std::endl;
      std::cout << "minI=" << minI << " \tmaxI=" << maxI << std::endl;
      std::cout << "confidence=" << confidence << " \tdx=" << dx << " \tdsize=" << dsize << std::endl;
  }
  void reset() {
      imageID = 0;
      x = 0;
      size = 0;
      minI = 0;
      maxI = 0;
      confidence = 0;
      dx = 0;
      dsize = 0;
  }
};


class Marker {
public:
  Marker(const Image3D& firstImage, bool isEnemy, const Rectangle &rect1, const Rectangle &rect2, const Rectangle &rect3);
  bool getNextPos(const Image3D& image, PositionMarker &nextPos);
  bool isMarkerFound(const PositionMarker& pm);
private:
  bool isEnemy;
  Rectangle rects[3];
public: // for debug
  Image masks[3];
private:
  static const unsigned int MIN_PM_HEIGHT = 15;
  static const unsigned int MAX_PM_HEIGHT = 1000;
  std::vector<unsigned char> masks_vec[3];
  Area previousAreas[3];
  PositionMarker previousPos;

  bool detectFromPoint(const Image3D& image, PositionMarker& nextPos, unsigned int startI, unsigned int startJ);
  bool detectFromZero(const Image3D& image, PositionMarker& nextPos);
  bool detectFromPrevious(const Image3D& image, PositionMarker& nextPost);
  void ratePositionMarker(const Image3D &image, PositionMarker& pm);

};

#endif //MARKER_HPP
