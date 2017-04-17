#include "gui.hpp"

GUI::GUI(Data& data)
    : data(data) {
}

void GUI::addRectangle(const PositionMarker& pm) {
    cv::rectangle(data.frame, cv::Point(pm.x - pm.size/2, pm.minI), cv::Point(pm.x + pm.size/2, pm.maxI), cv::Scalar(0, 0, 0));
    cv::rectangle(data.frame, cv::Point(pm.x - pm.size/2 + 1, pm.minI + 1), cv::Point(pm.x + pm.size/2 - 1, pm.maxI - 1), cv::Scalar(0, 0, 0));
}

void GUI::addMask(const Image& mask, const cv::Scalar& color) {
    for (unsigned int im=0; im<mask.height; ++im) {
        for (unsigned int jm=0; jm<mask.width; ++jm) {
            if (mask.getValue(im, jm) == 1) {
                cv::line(data.frame, cv::Point(jm, im), cv::Point(jm, im), color);
            }
        }
    }
}

void GUI::update(void) {
    if (data.gui_level == 0) {
        return;
    }

    // debug detected pixels for given data.marker/color
    addMask(data.marker[0].masks[0], cv::Scalar(255, 0, 0));
    addMask(data.marker[0].masks[1], cv::Scalar(0, 255, 0));
    addMask(data.marker[0].masks[2], cv::Scalar(0, 0, 255));
    // show detected data.markers
    addRectangle(data.pm[0]);

    // add middle acceptance lines:
    int middleI = data.image.height / 2;
    int accept = 30;
    cv::line(data.frame, cv::Point(0, middleI - accept), cv::Point(data.image.width-1, middleI - accept), cv::Scalar(0, 0, 0));
    cv::line(data.frame, cv::Point(0, middleI + accept), cv::Point(data.image.width-1, middleI + accept), cv::Scalar(0, 0, 0));

    // show the final image
    cv::imshow("img", data.frame);
    cv::waitKey(10);
//    if (data.image.id > 100)
//        cv::waitKey(500);
//    if (data.image.id > 175)
//        cv::waitKey(4500);
}
