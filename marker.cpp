#include "marker.hpp"
#include "util.hpp"
#include <iostream>
#include <cstdlib>

Marker::Marker(bool isEnemy, unsigned char hue1, unsigned char hue2, std::vector<bool> binaryCode) : isEnemy(isEnemy), binaryCode(binaryCode) {
  Color color1min(hue1 - hueAcceptance, saturationMin, valueMin);
  Color color1max(hue1 + hueAcceptance, saturationMax, valueMax);
  rects[0] = Rectangle(color1min, color1max);

  Color color2min(hue2 - hueAcceptance, saturationMin, valueMin);
  Color color2max(hue2 + hueAcceptance, saturationMax, valueMax);
  rects[1] = Rectangle(color2min, color2max);

  for (int i=0; i<2; ++i) {
    masks_vec[i].resize(640 * 480);
    masks[i] = Image(640, 480, masks_vec[i].data());
  }
}

void Marker::convertPositionMarker2Area(const Image3D& image, const PositionMarker& pm, Area& area, int rectID) const {
  area.minJ = pm.x - pm.size/2;
  area.maxJ = pm.x + pm.size/2;
  int coef;
  if (isEnemy) {
    coef = 1;
  } else {
    coef = -1;
  }
  int offset;
  if (rectID == 0) {
    offset = 0;
  } else {
    offset = pm.size;
  }
  unsigned int approxMiddleI = image.height/2 + coef*offset;
  area.minI = approxMiddleI - 10;
  area.maxI = approxMiddleI + 10;
}

void Marker::convertArea2PositionMarker(const Image3D& image, const Area& area, PositionMarker& pm, int rectID) const {
  pm.x = (area.minI + area.minI) / 2;
  pm.size = area.minI - area.minI;
}

bool Marker::getNextPos(const Image3D& image, PositionMarker &nextPos) {
  if (previousPos.imageID == 0) {
    previousPos.x = image.width/2;
    previousPos.size = image.width/10;
    for (unsigned int i=0; i<2; ++i) {
      convertPositionMarker2Area(image, previousPos, previousAreas[i], i);
    }
  }
  int dID = image.id - previousPos.imageID;
  std::cout << "dID = " << dID << std::endl;
  Area resultArea;
  for (int i=0; i<2; ++i) {
    masks[i].reset();
    unsigned int minJdone = previousAreas[i].minJ;
    unsigned int maxJdone = previousAreas[i].maxJ;
    std::cout << " :) " << i << std::endl;

    while (! rects[i].getArea(image, masks[i], previousAreas[i], resultArea)) {
      std::cout << previousAreas[i].minJ << " , " << previousAreas[i].maxJ << std::endl;
      if (! getOtherArea(image, previousAreas[i], minJdone, maxJdone)) {
	return false;
      }
    }

    std::cout << resultArea.minJ << " ; " << resultArea.maxJ << std::endl;
    std::cout << resultArea.error << " / " << resultArea.size << std::endl;
    for (int index=0; index<masks[i].width*masks[i].height; ++index) {
      if (masks[i].data[index] == 1) {
	std::cout << index << " \t";
      }
    }
    std::cout << "/end" << std::endl;
    previousAreas[i] = resultArea;
  }
  if (absdiff(previousAreas[0].minJ, previousAreas[1].minJ) < 10 &&
      absdiff(previousAreas[0].maxJ, previousAreas[1].maxJ) < 10) {
    nextPos.imageID = image.id;
    nextPos.x = (previousAreas[0].minJ + previousAreas[0].maxJ) / 2;
    nextPos.size = previousAreas[0].maxJ - previousAreas[0].minJ;
    nextPos.confidence = 1.f;
    nextPos.dx = nextPos.x - previousPos.x;
    nextPos.dsize = nextPos.size - previousPos.size;
    
    previousPos = nextPos;
    return true;
  }
  return false;
}

bool Marker::getOtherArea(const Image3D& image, Area &area, unsigned int& minJdone, unsigned int& maxJdone) {
  unsigned int dmin = previousPos.x - minJdone;
  unsigned int dmax = maxJdone - previousPos.x;
  if (dmin < dmax && minJdone > 10) {
    if (minJdone < previousPos.size)
      area.minJ = 0;
    else
      area.minJ = minJdone - previousPos.size;

    area.maxJ = minJdone;
    minJdone = area.minJ;
    return true;
  } else if (maxJdone < image.width - 10) {
    if (maxJdone < image.width - previousPos.size -1)
      area.maxJ = image.width-1;
    else
      area.maxJ = maxJdone + previousPos.size;

    area.minJ = maxJdone;
    maxJdone = area.maxJ;
    return true;
  } else {
    return false;
  }
}
