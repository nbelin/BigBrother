#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

#include "image.hpp"
#include "rectangle.hpp"
#include "list.hpp"
#include "marker.hpp"
#include "def_colors.hpp"

//#define VIDEO "sample/2015-06-06-172737.webm"
//#define VIDEO "sample/2015-06-06 17.29.40.mov"
//#define VIDEO "sample/2015-06-06-174454.webm"
#define VIDEO "sample/test.mp4"

int main(int argc, char* argv[]) {
  std::cout << "GO" << std::endl;
  cv::VideoCapture cap(VIDEO); // open the default camera
  if(!cap.isOpened())  // check if we succeeded
    return -1;
  
  cv::Mat frame;
  cv::Mat hsv;
  cap >> frame;
  
  Image3D image(frame.cols, frame.rows, NULL);

  Rectangle dummyRect;
  //Marker marker1(image, false, yellowRect, redRect, dummyRect);
  Marker marker1(image, true, darkBlueRect, magentaRect, dummyRect);
  PositionMarker pm1;
  Marker marker2(image, false, redRect, yellowRect, dummyRect);
  PositionMarker pm2;
  Marker marker3(image, true, greenRect, blueRect, dummyRect);
  PositionMarker pm3;
  Marker marker4(image, true, blueRect, greenRect, dummyRect);
  PositionMarker pm4;


  std::cout << "start loop" << std::endl;
  int count = 0;
  while(1) {
    cap >> frame; // get a new frame from camera
    cv::cvtColor(frame, hsv, CV_BGR2Luv);
    image.setData(hsv.data);
    image.id = count;

    if (count % 20 == 0)
        std::cout << "NEXT POS " << count << std::endl;
    bool result;

    result = marker1.getNextPos(image, pm1);
    if (!result) {
        std::cout << "POS " << count << std::endl;
        std::cout << "NOP (1)" << std::endl;
    }

//    result = marker2.getNextPos(image, pm2);
//    if (!result) {
//        std::cout << "POS " << count << std::endl;
//        std::cout << "NOP (2)" << std::endl;
//    }

//    result = marker3.getNextPos(image, pm3);
//    if (!result) {
//        std::cout << "POS " << count << std::endl;
//        std::cout << "NOP (3)" << std::endl;
//    }

//    result = marker4.getNextPos(image, pm4);
//    if (!result) {
//        std::cout << "POS " << count << std::endl;
//        std::cout << "NOP (4)" << std::endl;
//    }

     Image &mask = marker1.masks[0];
     for (unsigned int im=0; im<mask.height; ++im) {
       for (unsigned int jm=0; jm<mask.width; ++jm) {
        if (mask.getValue(im, jm) == 1)
          cv::line(frame, cv::Point(jm, im), cv::Point(jm, im), cv::Scalar(255, 0, 0));
       }
     }

//    Image &mask1 = marker3.masks[0];
//    for (unsigned int im=0; im<mask1.height; ++im) {
//      for (unsigned int jm=0; jm<mask1.width; ++jm) {
//        if (mask1.getValue(im, jm) == 1)
//          cv::line(frame, cv::Point(jm, im), cv::Point(jm, im), cv::Scalar(255, 0, 255));
//      }
//    }

    cv::rectangle(frame, cv::Point(pm1.x - pm1.size/2, pm1.minI), cv::Point(pm1.x + pm1.size/2, pm1.maxI), cv::Scalar(0, 0, 0));
    cv::rectangle(frame, cv::Point(pm1.x - pm1.size/2 + 1, pm1.minI + 1), cv::Point(pm1.x + pm1.size/2 - 1, pm1.maxI - 1), cv::Scalar(0, 0, 0));
    cv::rectangle(frame, cv::Point(pm2.x - pm2.size/2, pm2.minI), cv::Point(pm2.x + pm2.size/2, pm2.maxI), cv::Scalar(0, 0, 0));
    cv::rectangle(frame, cv::Point(pm2.x - pm2.size/2 + 1, pm2.minI + 1), cv::Point(pm2.x + pm2.size/2 - 1, pm2.maxI - 1), cv::Scalar(0, 0, 0));
    cv::rectangle(frame, cv::Point(pm3.x - pm3.size/2, pm3.minI), cv::Point(pm3.x + pm3.size/2, pm3.maxI), cv::Scalar(0, 0, 255));
    cv::rectangle(frame, cv::Point(pm3.x - pm3.size/2 + 1, pm3.minI + 1), cv::Point(pm3.x + pm3.size/2 - 1, pm3.maxI - 1), cv::Scalar(0, 0, 0));
    cv::rectangle(frame, cv::Point(pm4.x - pm4.size/2, pm4.minI), cv::Point(pm4.x + pm4.size/2, pm4.maxI), cv::Scalar(0, 0, 255));
    cv::rectangle(frame, cv::Point(pm4.x - pm4.size/2 + 1, pm4.minI + 1), cv::Point(pm4.x + pm4.size/2 - 1, pm4.maxI - 1), cv::Scalar(0, 0, 0));
//    cv::line(frame, cv::Point(pm1.x - pm1.size/2, 183), cv::Point(pm1.x + pm1.size/2, 211), cv::Scalar(0, 0, 255));
//    cv::line(frame, cv::Point(565, 220), cv::Point(575, 230), cv::Scalar(0, 0, 255));
    cv::imshow("img", frame);
    cv::waitKey(10);
    if (count > 472 && count < 475)
       cv::waitKey(1000);
    // if (count > 2250)
    //   cv::waitKey(2);
    
    count++;
  }
}
