#include <opencv2/opencv.hpp>
#include <vector>

/*
 * g++ main.cpp `pkg-config opencv --libs` --std=c++11
 */

// BGR
const cv::Vec3b G(0, 255, 167);
const cv::Vec3b B(255, 181, 0);
const cv::Vec3b M(222, 0, 255);

const std::vector<cv::Vec3b> marker1 = {M, G, B};
const std::vector<cv::Vec3b> marker2 = {M, B, G};
const std::vector<cv::Vec3b> marker3 = {G, M, B};
const std::vector<cv::Vec3b> marker4 = {G, B, M};

const std::vector<std::vector<cv::Vec3b>> markers = {marker1, marker2, marker3, marker4};

int main(int argc, char* argv[]) {
    char buffer[256];
    const int width = 1000;
    const int height = 300;
    cv::Mat mat(height, width, CV_8UC3);
    for (int id = 0; id < markers.size(); ++id) {
        for (int j = 0; j < width; j++) {
            for (int i = 0; i < height; i++) {
                int color_id = i / (height / 3);
                mat.at<cv::Vec3b>(i, j) = markers[id][color_id];
            }
        }

        snprintf(buffer, sizeof(buffer), "marker_%d.png", id);
        cv::imwrite(buffer, mat);
    }
}
