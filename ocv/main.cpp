/*
 * main.cpp
 *
 *  Created on: Dec 10, 2015
 *      Author: david
 */

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main( int argc, char** argv )
{
  Mat_<float> image = Mat::eye(Size(400, 400), CV_8UC1);

  imshow("img",image);

  waitKey(0);

  return 0;
}
