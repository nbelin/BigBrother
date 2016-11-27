#ifndef HSV_CONTROLLER_HPP
#define HSV_CONTROLLER_HPP

#include <vector>
#include <thread>
#include "data.hpp"

class HSVController {
private:
    Data& data;
    std::vector<std::thread> threads;
    std::vector<cv::Mat> src_mats;
    std::vector<cv::Mat> dst_mats;
public:
    HSVController(Data& data)
        : data(data) {
        threads.resize(4);
        src_mats.resize(4);
        dst_mats.resize(4);
        data.hsv = data.frame->clone();
    }

    void update(void) {
        if ((data.color_choice & Color::HSV) == 0) {
            return;
        }
#if 1
        cv::cvtColor(*data.frame, data.hsv, CV_BGR2HSV);
#else
        for (size_t i=0; i<threads.size(); ++i) {
            unsigned int height = data.frame->rows / 4;
            unsigned int row = height * i;
            cv::Rect rect = cv::Rect(0, row, data.frame->cols, height);
            src_mats[i] = (*data.frame)(rect);
            dst_mats[i] = data.hsv(rect);
            threads[i] = std::thread(jobCvtColor, &src_mats[i], &dst_mats[i]);
        }

        for (size_t i=0; i<threads.size(); ++i) {
            threads[i].join();
        }
#endif
        data.image.setData(data.hsv.data);
    }

    static void jobCvtColor(const cv::Mat * src, cv::Mat * dst) {
        cv::cvtColor(*src, *dst, CV_BGR2HSV);
    }

};

#endif //HSV_CONTROLLER_HPP
