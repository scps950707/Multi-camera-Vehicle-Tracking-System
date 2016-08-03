#ifndef __HEADER_HPP__
#define __HEADER_HPP__


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/opencv.hpp>

#include <stdio.h>
#include <iostream>
#include <getopt.h>
#include <vector>
#include <algorithm>
using namespace std;
#define FPS 30
#define BLACK_C1 cv::Scalar(0)
#define WHITE_C1 cv::Scalar(255)
#define GRAY_C1 cv::Scalar(127)
#define BLACK_C3 cv::Scalar(0,0,0)
#define WHITE_C3 cv::Scalar(255,255,255)
#define GRAY_C3 cv::Scalar(127,127,127)
#define RED_C3 cv::Scalar(0,0,255)
#define BLUE_C3 cv::Scalar(255,0,0)
#define GREEN_C3 cv::Scalar(0,255,0)


#endif
