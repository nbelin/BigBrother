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
  for (int i=0; i<2; ++i) {
    memset(masks[i].data, 0, masks[i].width * masks[i].height);
  }
  return detectFromZero(image, nextPos);

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
    for (unsigned int index=0; index<masks[i].width*masks[i].height; ++index) {
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

bool Marker::detectFromZero(const Image3D& image, PositionMarker& nextPos) {
  for (unsigned int i=0; i<image.height; i++) {
    for (unsigned int j=0; j<image.width; j++) {
      if (masks[0].getValue(i, j) == 0 && rects[0].isPixelRightColor(image, i, j)) {
	rects[0].expandArea(image, masks[0], previousAreas[0], i, j);
	rects[0].rateArea(previousAreas[0]);
	if (previousAreas[0].size > 50 && previousAreas[0].rank > 2) {
	  // std::cout << previousAreas[0].size << "\t";
	  // std::cout << previousAreas[0].error << "\t";
	  // std::cout << previousAreas[0].rank << std::endl;
	  const unsigned int length = previousAreas[0].maxJ - previousAreas[0].minJ;
	  const unsigned int nj = previousAreas[0].minJ + length/2;
	  for (unsigned int ni = i; ni < i + length + 10; ++ni) {
	    if (rects[1].isPixelRightColor(image, ni, nj)) {
	      rects[1].expandArea(image, masks[1], previousAreas[1], ni, nj);
	      // std::cout << previousAreas[1].size << "\t";
	      // std::cout << previousAreas[1].minJ << "\t";
	      // std::cout << previousAreas[1].maxJ << " ? " << std::endl;
	      if (absdiff(previousAreas[0].minJ, previousAreas[1].minJ) < 30 &&
	  	  absdiff(previousAreas[0].maxJ, previousAreas[1].maxJ) < 30) {
		nextPos.imageID = image.id;
		nextPos.x = nj;
		nextPos.size = length;
		nextPos.confidence = previousAreas[0].rank;
		nextPos.dx = 0;
		nextPos.dsize = 0;
	  	if (checkBinary(image, nextPos)) {
		  return true;
		}
	      }
	    }
	  }
	}
      }
    }
  }
  return false;
}

bool Marker::checkBinary(const Image3D& image, PositionMarker& nextPos) {
  const unsigned int j = (previousAreas[0].minJ + previousAreas[0].maxJ) / 2;
  const unsigned int startI = previousAreas[0].maxI + 1;
  const unsigned int endI = previousAreas[1].minI - 1;
  int index=0;
  bool current, old = (image.getValue(startI, j).v3) < 128;
  if (binaryCode[index] != old) {
    return false;
  }
  for (unsigned int i = startI; i<endI; ++i) {
    if ((current = image.getValue(i, j).v3 < 128) != old) {
      index++;
      if (binaryCode[index] != current) {
	return false;
      }
      old = current;
    }
  }
  return index == binaryCode.size() - 1;
}
