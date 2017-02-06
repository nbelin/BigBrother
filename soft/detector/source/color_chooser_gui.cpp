#include "color_chooser_gui.hpp"

cv::Mat frame;
int id_1 = 0;
int id_2 = 1;
int id_3 = 2;
int radius_1 = 20;
int radius_2 = 20;
int radius_3 = 20;
int lthreshold_1 = 50;
int lthreshold_2 = 50;
int lthreshold_3 = 50;
int h_accept_1 = 10;
int h_accept_2 = 10;
int h_accept_3 = 10;
int s_accept_1 = 10;
int s_accept_2 = 10;
int s_accept_3 = 10;
int v_min_1 = 100;
int v_min_2 = 100;
int v_min_3 = 100;
int key;
int couleur = 0;
int tab_couleur_luv[10][3] = {0};
int tab_couleur_hsv[10][3] = {0};

void CallBackFunc(int event, int x, int y, int, void* userdata) {
    Data& data = *(Data*)userdata;
    if(event == cv::EVENT_LBUTTONDOWN) {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
        cv::Vec3b color;
        color = data.luv.at<cv::Vec3b>(y, x);
        tab_couleur_luv[couleur][0] = color.val[0];
        tab_couleur_luv[couleur][1] = color.val[1];
        tab_couleur_luv[couleur][2] = color.val[2];
        std::cout << "L=" << tab_couleur_luv[couleur][0] << " U=" << tab_couleur_luv[couleur][1] << " V=" << tab_couleur_luv[couleur][2] << std::endl;
        color = data.hsv.at<cv::Vec3b>(y, x);
        tab_couleur_hsv[couleur][0] = color.val[0];
        tab_couleur_hsv[couleur][1] = color.val[1];
        tab_couleur_hsv[couleur][2] = color.val[2];
        std::cout << "H=" << tab_couleur_hsv[couleur][0] << " S=" << tab_couleur_hsv[couleur][1] << " V=" << tab_couleur_hsv[couleur][2] << std::endl;
        couleur++;
    }
}


void CallBackFunc_diagLUV(int event, int x, int y, int, void* userdata) {
    int * id = (int*)userdata;
    if (*id >= couleur) return;
    if(event == cv::EVENT_LBUTTONDOWN) {
        std::cout << "Change color of id " << *id << " (" << x << ", " << y << ")" << std::endl;
        tab_couleur_luv[*id][0] = 255;
        tab_couleur_luv[*id][1] = y;
        tab_couleur_luv[*id][2] = x;
    }
}


void CallBackFunc_diagHSV(int event, int x, int y, int, void* userdata) {
    int * id = (int*)userdata;
    if (*id >= couleur) return;
    if(event == cv::EVENT_LBUTTONDOWN) {
        std::cout << "Change color of id " << *id << " (" << x << ", " << y << ")" << std::endl;
        tab_couleur_hsv[*id][0] = y;
        tab_couleur_hsv[*id][1] = x;
        tab_couleur_hsv[*id][2] = 255;
    }
}


int color_square_diff(cv::Vec3b color1, cv::Vec3b color2) {
    //const unsigned char l1 = color1.val[0];
    const unsigned char u1 = color1.val[1];
    const unsigned char v1 = color1.val[2];
    //const unsigned char l2 = color2.val[0];
    const unsigned char u2 = color2.val[1];
    const unsigned char v2 = color2.val[2];
    //const unsigned int diff_l = l1 - l2;
    const unsigned int diff_u = absdiff(u1, u2);
    const unsigned int diff_v = absdiff(v1, v2);
    return diff_u * diff_u + diff_v * diff_v;// + diff_l * diff_l;
}


cv::Mat show_color_LUV(cv::Mat image, cv::Mat image_LUV, int id, int r, int l) {
    cv::Mat masque = cv::Mat::zeros(image.size().height, image.size().width, CV_8U);

    cv::Vec3b cref(tab_couleur_luv[id][0], tab_couleur_luv[id][1], tab_couleur_luv[id][2]);

    for(int j = 0; j < image_LUV.size().width; j++) {
        for(int i = 0; i < image_LUV.size().height; i++) {
            masque.at<uchar>(i, j) = image_LUV.at<cv::Vec3b>(i, j).val[0] / 2;

            cv::Vec3b c1 = image_LUV.at<cv::Vec3b>(i, j);
            int diff = color_square_diff(c1, cref);
            if(diff <= r*r && c1.val[0] >= l) {
                masque.at<uchar>(i,j) = 255;
            }
        }
    }

    return masque;
}


cv::Mat show_diagram_LUV(int id, int r, int l) {
    cv::Mat masque = cv::Mat::zeros(255, 255, CV_8UC3);

    cv::Vec3b cref(tab_couleur_luv[id][0], tab_couleur_luv[id][1], tab_couleur_luv[id][2]);

    for(int j = 0; j < masque.size().width; j++) {
        for(int i = 0; i < masque.size().height; i++) {
            masque.at<cv::Vec3b>(i, j) = cv::Vec3b(l, i, j);

            cv::Vec3b c1 = cv::Vec3b(l, i, j);
            int diff = color_square_diff(cref, c1);
            if ((diff - (r * r)) <= 20 && (diff - (r * r)) >= -20) {
                masque.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
            }
        }
    }

    cvtColor(masque, masque, CV_Luv2BGR);

    return masque;
}


cv::Mat show_color_HSV(cv::Mat image, cv::Mat image_HSV, int id, int h_acc, int s_acc, int v_min) {
    cv::Mat masque = cv::Mat::zeros(image.size().height, image.size().width, CV_8U);

    int href = tab_couleur_hsv[id][0];
    int sref = tab_couleur_hsv[id][1];
    int hmin = href - h_acc;
    int hmax = href + h_acc;
    int smin = sref - s_acc;
    int smax = sref + s_acc;

    for(int j = 0; j < image_HSV.size().width; j++) {
        for(int i = 0; i < image_HSV.size().height; i++) {
            masque.at<uchar>(i, j) = image_HSV.at<cv::Vec3b>(i, j).val[2] / 3;

            int h = image_HSV.at<cv::Vec3b>(i, j).val[0];
            int s = image_HSV.at<cv::Vec3b>(i, j).val[1];
            int v = image_HSV.at<cv::Vec3b>(i, j).val[2];
            if (h >= hmin && h <= hmax && s >= smin && s <= smax && v >= v_min) {
                masque.at<cv::Vec3b>(i, j) = cv::Vec3b(i, j, 255);
            }
        }
    }

    return masque;
}


cv::Mat show_diagram_HSV(int id, int h_acc, int s_acc) {
    cv::Mat masque = cv::Mat::zeros(255, 255, CV_8UC3);

    int href = tab_couleur_hsv[id][0];
    int sref = tab_couleur_hsv[id][1];
    int hmin = href - h_acc;
    int hmax = href + h_acc;
    int smin = sref - s_acc;
    int smax = sref + s_acc;

    for(int j = 0; j < masque.size().width; j++) {
        for(int i = 0; i < masque.size().height; i++) {
            if (i != hmin && i != hmax && j != smin && j != smax) {
                masque.at<cv::Vec3b>(i, j) = cv::Vec3b(i, j, 255);
            }
        }
    }

    cvtColor(masque, masque, CV_HSV2BGR);

    return masque;
}


ColorChooserGUI::ColorChooserGUI(Data& data)
    : data(data) {
    cv::namedWindow("frameToClick");
    cv::namedWindow("frameLUV_1");
    cv::namedWindow("frameLUV_2");
    cv::namedWindow("frameLUV_3");
    cv::namedWindow("frameHSV_1");
    cv::namedWindow("frameHSV_2");
    cv::namedWindow("frameHSV_3");
    cv::namedWindow("diagLUV_1");
    cv::namedWindow("diagLUV_2");
    cv::namedWindow("diagLUV_3");
    cv::namedWindow("diagHSV_1");
    cv::namedWindow("diagHSV_2");
    cv::namedWindow("diagHSV_3");
    cv::createTrackbar("square radius color acceptance", "diagLUV_1", &radius_1, 150);
    cv::createTrackbar("square radius color acceptance", "diagLUV_2", &radius_2, 150);
    cv::createTrackbar("square radius color acceptance", "diagLUV_3", &radius_3, 150);
    cv::createTrackbar("luminance threshold", "diagLUV_1", &lthreshold_1, 200);
    cv::createTrackbar("luminance threshold", "diagLUV_2", &lthreshold_2, 200);
    cv::createTrackbar("luminance threshold", "diagLUV_3", &lthreshold_3, 200);
    cv::createTrackbar("hue acceptance", "diagHSV_1", &h_accept_1, 50);
    cv::createTrackbar("hue acceptance", "diagHSV_2", &h_accept_2, 50);
    cv::createTrackbar("hue acceptance", "diagHSV_3", &h_accept_3, 50);
    cv::createTrackbar("saturation acceptance", "diagHSV_1", &s_accept_1, 50);
    cv::createTrackbar("saturation acceptance", "diagHSV_2", &s_accept_2, 50);
    cv::createTrackbar("saturation acceptance", "diagHSV_3", &s_accept_3, 50);
    cv::createTrackbar("minimum value", "diagHSV_1", &v_min_1, 200);
    cv::createTrackbar("minimum value", "diagHSV_2", &v_min_2, 200);
    cv::createTrackbar("minimum value", "diagHSV_3", &v_min_3, 200);

    cv::setMouseCallback("frameToClick", CallBackFunc, &data);
    cv::setMouseCallback("diagLUV_1", CallBackFunc_diagLUV, &id_1);
    cv::setMouseCallback("diagLUV_2", CallBackFunc_diagLUV, &id_2);
    cv::setMouseCallback("diagLUV_3", CallBackFunc_diagLUV, &id_3);
    cv::setMouseCallback("diagHSV_1", CallBackFunc_diagHSV, &id_1);
    cv::setMouseCallback("diagHSV_2", CallBackFunc_diagHSV, &id_2);
    cv::setMouseCallback("diagHSV_3", CallBackFunc_diagHSV, &id_3);
}


void ColorChooserGUI::update(void) {
    cv::imshow("frameToClick", data.frame);
    //cv::imshow("data.hsv", data.hsv);
    if ( couleur >= 1 ) {
        // LUV
        frame = show_color_LUV(data.frame, data.luv, 0, radius_1, lthreshold_1);
        cv::imshow("frameLUV_1", frame);
        frame = show_diagram_LUV(0, radius_1, lthreshold_1);
        cv::imshow("diagLUV_1", frame);
        // HSV
        frame = show_color_HSV(data.frame, data.hsv, 0, h_accept_1, s_accept_1, v_min_1);
        cv::imshow("frameHSV_1", frame);
        frame = show_diagram_LUV(0, h_accept_1, s_accept_1);
        cv::imshow("diagHSV_1", frame);
    }
    if ( couleur >= 2 ) {
        // LUV
        frame = show_color_LUV(data.frame, data.luv, 1, radius_2, lthreshold_2);
        cv::imshow("frameLUV_2", frame);
        frame = show_diagram_LUV(1, radius_2, lthreshold_2);
        cv::imshow("diagLUV_2", frame);
        // HSV
        frame = show_color_HSV(data.frame, data.hsv, 1, h_accept_2, s_accept_2, v_min_2);
        cv::imshow("frameHSV_2", frame);
        frame = show_diagram_LUV(1, h_accept_2, s_accept_2);
        cv::imshow("diagHSV_2", frame);
    }
    if ( couleur >= 3 ) {
        // LUV
        frame = show_color_LUV(data.frame, data.luv, 2, radius_3, lthreshold_3);
        cv::imshow("frameLUV_3", frame);
        frame = show_diagram_LUV(2, radius_3, lthreshold_3);
        cv::imshow("diagLUV_3", frame);
        // HSV
        frame = show_color_HSV(data.frame, data.hsv, 2, h_accept_3, s_accept_3, v_min_3);
        cv::imshow("frameHSV_3", frame);
        frame = show_diagram_LUV(2, h_accept_3, s_accept_3);
        cv::imshow("diagHSV_3", frame);
    }
}
