#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <vector>
#include <threads>

#include "marker.hpp"
#include "image.hpp"

class Manager {
public:
  Manager();
  ~Manager();
  bool addMarker(Marker &marker);
  void go(void);

private:
  static const unsigned int NB_FRAMES = 50;
  Image3D
  unsigned int currentFrame;
  std::vector<std::threads> poolThreads;
  std::vector<Marker&> poolMarkers;
};

#endif //MANAGER_HPP
