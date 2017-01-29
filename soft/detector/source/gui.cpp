#include "gui.hpp"

GUI::GUI(void)
    : _frame(0) {

}

void GUI::setFrame(cv::Mat& frame) {
    _frame = &frame;
}

void GUI::addRectangle(const PositionMarker& pm) {
  if(_frame) {
    cv::rectangle(*_frame, cv::Point(pm.x - pm.size/2, pm.minI), cv::Point(pm.x + pm.size/2, pm.maxI), cv::Scalar(0, 0, 0));
    cv::rectangle(*_frame, cv::Point(pm.x - pm.size/2 + 1, pm.minI + 1), cv::Point(pm.x + pm.size/2 - 1, pm.maxI - 1), cv::Scalar(0, 0, 0));
  }
}

void GUI::addMask(const Image& mask) {
  if(_frame) {
    for (unsigned int im=0; im<mask.height; ++im) {
      for (unsigned int jm=0; jm<mask.width; ++jm) {
        if (mask.getValue(im, jm) == 1) {
          cv::line(*_frame, cv::Point(jm, im), cv::Point(jm, im), cv::Scalar(255, 0, 0));
        }
      }
    }
  }
}

void GUI::update(void) {
  if(_frame) {
    cv::imshow("img", *_frame);
    cv::waitKey(100);
  }
}
