#include "image.hpp"
#include "rectangle.hpp"
#include <vector>

class PositionMarker {
public:
  PositionMarker(void) : imageID(0), x(0), size(0), confidence(0), dx(0), dsize(0) {}
  unsigned int imageID;
  unsigned int x;
  unsigned int size;
  float confidence;
  int dx;
  int dsize;
};


class Marker {
public:
  Marker(bool isEnemy, unsigned char hue1, unsigned char hue2, std::vector<bool> binaryCode);
  void convertPositionMarker2Area(const Image3D& image, const PositionMarker& pm, Area& area, int rectID) const;
  void convertArea2PositionMarker(const Image3D& image, const Area& area, PositionMarker& pm, int rectID) const;
  bool getNextPos(const Image3D& image, PositionMarker &nextPos);
private:
  static const unsigned char hueAcceptance = 8;
  static const unsigned char saturationMin = 60;
  static const unsigned char saturationMax = 255;
  static const unsigned char valueMin = 60;
  static const unsigned char valueMax = 255;

  bool isEnemy;
  Rectangle rects[2];
public:
  Image masks[2];
private:
  std::vector<unsigned char> masks_vec[2];
  Area previousAreas[2];
  std::vector<bool> binaryCode;
  PositionMarker previousPos;

  bool getOtherArea(const Image3D& image, Area &area, unsigned int& minJdone, unsigned int& maxJdone);
};

