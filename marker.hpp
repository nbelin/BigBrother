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
  Marker(bool isEnemy, Color color1min, Color color1max, Color color2min, Color color2max, std::vector<bool> binaryCode);
  void convertPositionMarker2Area(const Image3D& image, const PositionMarker& pm, Area& area, int rectID) const;
  void convertArea2PositionMarker(const Image3D& image, const Area& area, PositionMarker& pm, int rectID) const;
  bool getNextPos(const Image3D& image, PositionMarker &nextPos);
private:
  bool isEnemy;
  Rectangle rects[2];
public:
  Image masks[2];
private:
  static const unsigned int MIN_AREA_SIZE = 60;
  static const unsigned int MAX_AREA_SIZE = 3000;
  std::vector<unsigned char> masks_vec[2];
  Area previousAreas[2];
  std::vector<bool> binaryCode;
  PositionMarker previousPos;

  bool getOtherArea(const Image3D& image, Area &area, unsigned int& minJdone, unsigned int& maxJdone);
  bool detectFromPoint(const Image3D& image, PositionMarker& nextPos, unsigned int startI, unsigned int startJ);
  bool detectFromZero(const Image3D& image, PositionMarker& nextPos);
  bool checkBinary(const Image3D& image, PositionMarker& nextPos);
};

