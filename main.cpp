#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

#include "image.hpp"
#include "rectangle.hpp"
#include "list.hpp"
#include "marker.hpp"

#define VIDEO "sample/2015-06-06-172737.webm"

int main(int argc, char* argv[]) {
  std::cout << "GO" << std::endl;
  cv::VideoCapture cap(VIDEO); // open the default camera
  if(!cap.isOpened())  // check if we succeeded
    return -1;
  
  cv::Mat frame;
  cv::Mat hsv;
  cap >> frame;
  
  Image3D image(frame.cols, frame.rows, NULL);
  std::vector<bool> binary;
  binary.push_back(1);
  binary.push_back(0);
  binary.push_back(1);
  binary.push_back(0);
  Marker marker1(false, 30, 165, binary);
  //Marker marker1(false, 165, 30, binary);
  PositionMarker pm1;

  std::cout << "start loop" << std::endl;
  int count = 0;
  while(1) {
    cap >> frame; // get a new frame from camera
    cv::cvtColor(frame, hsv, CV_BGR2HSV);
    image.setData(hsv.data);
    image.id = count;

    std::cout << "NEXT POS " << count << std::endl;
    bool result = marker1.getNextPos(image, pm1);
    if (!result)
      std::cout << "NOP" << std::endl;
    /*    
    Image &mask = marker1.masks[0];
    for (unsigned int im=0; im<mask.height; ++im) {
      for (unsigned int jm=0; jm<mask.width; ++jm) {
    	if (mask.getValue(im, jm) == 1)
    	  cv::line(frame, cv::Point(jm, im), cv::Point(jm, im), cv::Scalar(255, 0, 0));
      }
    }
    Image &mask1 = marker1.masks[1];
    for (unsigned int im=0; im<mask1.height; ++im) {
      for (unsigned int jm=0; jm<mask1.width; ++jm) {
    	if (mask1.getValue(im, jm) == 1)
    	  cv::line(frame, cv::Point(jm, im), cv::Point(jm, im), cv::Scalar(255, 255, 0));
      }
    }
    cv::rectangle(frame, cv::Point(pm1.x - pm1.size/2, 100), cv::Point(pm1.x + pm1.size/2, 400), cv::Scalar(0, 0, 0));
    cv::imshow("img", frame);
    cv::waitKey(3);
    if (count > 84)
      cv::waitKey(10);
    if (count > 2250)
      cv::waitKey(2);
    */
    count++;
  }
}
