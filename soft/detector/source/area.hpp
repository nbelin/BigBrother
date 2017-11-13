#ifndef AREA_HPP
#define AREA_HPP

class Area {
public:
  unsigned int minI;
  unsigned int maxI;
  unsigned int minJ;
  unsigned int maxJ;
  unsigned int size;
  unsigned int width;
  unsigned int height;
  unsigned int count;
  float rank; // 0 to 1
  inline bool isInside(unsigned int i, unsigned int j) const {
    return (i >= minI && i <= maxI &&
        j >= minJ && j <= maxJ);
  }
  void display() {
      std::cout << "(" << minI << ", " << minJ << ") \t -> (" << maxI << ", " << maxJ << ")\n";
      std::cout << "size=" << size << " \tw=" << width << " \th=" << height << "\n";
      std::cout << "count=" << count << " \trank=" << rank << std::endl;
  }
};

#endif//AREA_HPP
