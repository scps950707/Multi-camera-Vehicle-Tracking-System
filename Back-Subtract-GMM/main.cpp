#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "bsgmm.hpp"
using namespace std;

/*
 * Author:scps950707
 * Email:scps950707@gmail.com
 * Date:2016-06-04
 * Filename:main.cpp
 * Purpose:An example to use Background Subtraction using Gaussian Mixture Models
 */

/**
 *\file main.cpp
 *\brief An example to use Background Subtraction using Gaussian Mixture Models
 *
 */

int main( int argc, char *argv[] )
{
  cv::Mat inputImg, outputImg;
  cv::VideoCapture capture( argv[1] );
  if ( !capture.read( inputImg ) )
  {
    cout << " Can't recieve input from source " << endl;
    exit( EXIT_FAILURE );
  }
  cv::cvtColor( inputImg, inputImg, CV_BGR2YCrCb );
  outputImg = cv::Mat( inputImg.rows, inputImg.cols, CV_8U, cv::Scalar( 0 ) );
  BackgroundSubtractorGMM bsgmm(  inputImg.rows, inputImg.cols );
  bsgmm.initFirstFrame( inputImg.ptr() );
  while ( capture.read( inputImg ) )
  {
    bsgmm.updateFrame( inputImg.ptr(), outputImg.ptr() );
    cv::imshow( "video", inputImg );
    cv::imshow( "GMM", outputImg );
    if ( cv::waitKey( 1 ) > 0 )
    {
      break;
    }
  }
  bsgmm.freeMem();
  return EXIT_SUCCESS;
}
