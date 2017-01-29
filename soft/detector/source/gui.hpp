#ifndef GUI_HPP
#define GUI_HPP

#include <opencv2/opencv.hpp>
#include "marker.hpp"
#include "image.hpp"

class GUI {
private:
    cv::Mat* _frame;
public:
    GUI(void);
    void setFrame(cv::Mat& frame);
    void addRectangle(const PositionMarker& pm);
    void addMask(const Image& msk);
    void update(void);
};

#endif//GUI_HPP
