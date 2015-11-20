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
  Marker marker1(false, 30, 60, binary);
  PositionMarker pm1;

  std::cout << "start loop" << std::endl;
  int count = 0;
  while(1) {
    cap >> frame; // get a new frame from camera
    cv::cvtColor(frame, hsv, CV_BGR2HSV);
    image.setData(hsv.data);

    std::cout << "NEXT POS" << std::endl;
    marker1.getNextPos(image, pm1);
    
    Image &mask = marker1.masks[0];
    for (int im=0; im<mask.height; ++im) {
      for (int jm=0; jm<mask.width; ++jm) {
    	if (mask.getValue(im, jm) == 1)
    	  cv::line(frame, cv::Point(jm, im), cv::Point(jm, im), cv::Scalar(255, 0, 0));
      }
    }
    cv::rectangle(frame, cv::Point(pm1.x - pm1.size/2, 100), cv::Point(pm1.x + pm1.size/2, 400), cv::Scalar(0, 0, 0));
    cv::imshow("img", frame);
    cv::waitKey(1000);
    if (count > 2250)
      cv::waitKey(2);
    count++;
    if (count % 64 == 0)
      std::cout << count << std::endl;
  }
}
