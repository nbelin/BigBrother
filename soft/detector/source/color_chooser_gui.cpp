#include "color_chooser_gui.hpp"

cv::Mat frame_1, frame_2, frame_3;
int radius_1 = 20;
int radius_2 = 20;
int radius_3 = 20;
int key;
int couleur = 0;
int tab_couleur[10][3] = {0};

void CallBackFunc(int event, int x, int y, int, void* userdata) {
    Data& data = *(Data*)userdata;
    if(event == cv::EVENT_LBUTTONDOWN) {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
        cv::Vec3b color = data.hsv.at<cv::Vec3b>(y, x);
        tab_couleur[couleur][0] = color.val[0];
        tab_couleur[couleur][1] = color.val[1];
        tab_couleur[couleur][2] = color.val[2];
        std::cout << "L=" << tab_couleur[couleur][0] << " U=" << tab_couleur[couleur][1] << " V=" << tab_couleur[couleur][2] << std::endl;
        couleur++;
    }
}

int color_square_diff(cv::Vec3b color1, cv::Vec3b color2) {
    //const char l1 = color1.val[0];
    const char u1 = color1.val[1];
    const char v1 = color1.val[2];
    //const char l2 = color2.val[0];
    const char u2 = color2.val[1];
    const char v2 = color2.val[2];
    //const int diff_l = l1 - l2;
    const int diff_u = u1 - u2;
    const int diff_v = v1 - v2;
    return diff_u * diff_u + diff_v * diff_v;// + diff_l * diff_l;
}

cv::Mat show_color(cv::Mat image, cv::Mat image_LUV, int id, int r) {
    cv::Mat masque = cv::Mat::zeros(image.size().height, image.size().width, CV_8U);

    for(int j = 0; j < image_LUV.size().width; j++) {
        for(int i = 0; i < image_LUV.size().height; i++) {
            masque.at<uchar>(i, j) = image_LUV.at<cv::Vec3b>(i, j).val[0] / 2;

            cv::Vec3b c1 = image_LUV.at<cv::Vec3b>(i, j);
            cv::Vec3b c2(tab_couleur[id][0], tab_couleur[id][1], tab_couleur[id][2]);
            int diff = color_square_diff(c1, c2);
            if(diff <= r*r && c1.val[0] > 50) {
                masque.at<uchar>(i,j) = 255;
            }
        }
    }

    return masque;
}


ColorChooserGUI::ColorChooserGUI(Data& data)
    : data(data) {
    cv::namedWindow("frame");
    cv::namedWindow("frame_1");
    cv::namedWindow("frame_2");
    cv::namedWindow("frame_3");
    cv::createTrackbar("square radius color acceptance", "frame_1", &radius_1, 100);
    cv::createTrackbar("square radius color acceptance", "frame_2", &radius_2, 100);
    cv::createTrackbar("square radius color acceptance", "frame_3", &radius_3, 100);
}

void ColorChooserGUI::update(void) {
    cv::setMouseCallback("frame", CallBackFunc, &data);

    cv::imshow("frame", data.frame);
    //cv::imshow("data.hsv", data.hsv);
    if ( couleur >= 1 ) {
        frame_1 = show_color(data.frame, data.hsv, 0, radius_1);
        cv::imshow("frame_1", frame_1);
    }
    if ( couleur >= 2 ) {
        frame_2 = show_color(data.frame, data.hsv, 1, radius_2);
        cv::imshow("frame_2", frame_2);
    }
    if ( couleur >= 3 ) {
        frame_3 = show_color(data.frame, data.hsv, 2, radius_3);
        cv::imshow("frame_3", frame_3);
    }
}
