#ifndef MARKER_HPP
#define MARKER_HPP

#include "image.hpp"
#include "rectangle.hpp"
#include <vector>
#include "position_marker.hpp"

class Marker {
public:
    Marker(const Image3D& firstImage, bool isEnemy, const ColorSet& rect1, const ColorSet& rect2, const ColorSet& rect3);
    Marker(const Image3D& firstImage, bool isEnemy, const Rectangle &rect1, const Rectangle &rect2, const Rectangle &rect3);
    bool getNextPos(int color_mode, const Image3D& image, PositionMarker &nextPos);
    bool isMarkerFound(const PositionMarker& pm);

private:
    bool isEnemy;
    Rectangle rects[3];
public: // for debug
    Image masks[3];
private:
    static const unsigned int MIN_PM_HEIGHT = 10;
    static const unsigned int MAX_PM_HEIGHT = 200;
    std::vector<unsigned char> masks_vec[3];
    Area previousAreas[3];
    PositionMarker previousPos;
    int colorMode;

    bool detectFromPoint(const Image3D& image, PositionMarker& nextPos, unsigned int startI, unsigned int startJ);
    bool detectFromZero(const Image3D& image, PositionMarker& nextPos);
    bool detectFromPrevious(const Image3D& image, PositionMarker& nextPost);
    void ratePositionMarker(const Image3D &image, PositionMarker& pm);

};

#endif //MARKER_HPP
