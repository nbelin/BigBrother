#include "gui.hpp"

GUI::GUI(Data& data)
    : data(data) {

}

void GUI::addRectangle(const PositionMarker& pm) {
    cv::rectangle(data.frame, cv::Point(pm.x - pm.size/2, pm.minI), cv::Point(pm.x + pm.size/2, pm.maxI), cv::Scalar(0, 0, 0));
    cv::rectangle(data.frame, cv::Point(pm.x - pm.size/2 + 1, pm.minI + 1), cv::Point(pm.x + pm.size/2 - 1, pm.maxI - 1), cv::Scalar(0, 0, 0));
}

void GUI::addMask(const Image& mask) {
    for (unsigned int im=0; im<mask.height; ++im) {
        for (unsigned int jm=0; jm<mask.width; ++jm) {
            if (mask.getValue(im, jm) == 1) {
                cv::line(data.frame, cv::Point(jm, im), cv::Point(jm, im), cv::Scalar(255, 0, 0));
            }
        }
    }
}

void GUI::update(void) {
    // debug detected pixels for given data.marker/color
    addMask(data.marker[0].masks[0]);
    addMask(data.marker[0].masks[1]);
    addMask(data.marker[0].masks[2]);
    // show detected data.markers
    addRectangle(data.pm[0]);

    // show the final image
    cv::imshow("img", data.frame);
    cv::waitKey(100);
}
