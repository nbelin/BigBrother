#ifndef POSITION_MARKER_HPP
#define POSITION_MARKER_HPP

class PositionMarker {
public:
  PositionMarker(int pmID = 0) : pmID(pmID), imageID(0), x(0), size(0), confidence(0), dx(0), dsize(0) {}
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
  bool hasBeenFound() {
      return confidence > 0;
  }
};


#endif//POSITION_MARKER_HPP
