#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/features2d/features2d.hpp>
#include <stdlib.h>
#include "bsgmm.hpp"
#include <vector>
using namespace std;
//Temperory variable
int overall = 0;
gaussian *ptr, *head, *tail,  *temp_ptr;
NODE *pixelGaussianBuffer,*pixelPtr;
//Some function associated with the structure management
NODE Create_Node( double info1, double info2, double info3 )
{
  NODE tmp;
  tmp.numComponents = 1;
  tmp.pixel_r = tmp.pixel_s = Create_gaussian( info1, info2, info3 );
  return tmp;
}
gaussian *Create_gaussian( double info1, double info2, double info3 )
{
  ptr = new gaussian;
  if ( ptr != NULL )
  {
    ptr->mean[0] = info1;
    ptr->mean[1] = info2;
    ptr->mean[2] = info3;
    ptr->covariance = covariance0;
    ptr->weight = alpha;
    ptr->next = NULL;
    ptr->prev = NULL;
  }
  return ptr;
}
gaussian *Delete_gaussian( gaussian *nptr )
{
  gaussian *previous, *nextptr;
  previous = nptr->prev;
  nextptr = nptr->next;
  if ( head != NULL )
  {
    if ( nptr == head && nptr == tail )
    {
      head = tail = NULL;
      delete nptr;
    }
    else if ( nptr == head )
    {
      nextptr->prev = NULL;
      head = nextptr;
      delete nptr;
      nptr = head;
    }
    else if ( nptr == tail )
    {
      previous->next = NULL;
      tail = previous;
      delete nptr;
      nptr = tail;
    }
    else
    {
      previous->next = nextptr;
      nextptr->prev = previous;
      delete nptr;
      nptr = nextptr;
    }
  }
  else
  {
    std::cout << "Underflow........";
    exit( EXIT_FAILURE );
  }
  return nptr;
}
int main( int argc, char *argv[] )
{
  // Declare matrices to store original and resultant binary image
  cv::Mat inputImg, outputImg;
  //Declare a VideoCapture object to store incoming frame and initialize it
  cv::VideoCapture capture( argv[1] );
  //Checking if input source is valid
  if ( !capture.read( inputImg ) )
  {
    std::cout << " Can't recieve input from source ";
    exit( EXIT_FAILURE );
  }
  cv::cvtColor( inputImg, inputImg, CV_BGR2YCrCb );
  //Initializing the binary image with the same dimensions as original image
  outputImg = cv::Mat( inputImg.rows, inputImg.cols, CV_8U, cv::Scalar( 0 ) );
  uchar *inputPtr;
  uchar *outputPtr;
  pixelGaussianBuffer = new NODE[inputImg.rows * inputImg.cols];
  for ( int i = 0; i < inputImg.rows; i++ )
  {
    inputPtr = inputImg.ptr( i );
    for ( int j = 0; j < inputImg.cols; j++ )
    {
      pixelGaussianBuffer[i * inputImg.cols + j] = Create_Node( *inputPtr, *( inputPtr + 1 ), *( inputPtr + 2 ) );
      pixelGaussianBuffer[i * inputImg.cols + j].pixel_s->weight = 1.0;
    }
  }
  capture.read( inputImg );
  int nL, nC;
  if ( inputImg.isContinuous() == true )
  {
    nL = 1;
    nC = inputImg.rows * inputImg.cols * inputImg.channels();
  }
  else
  {
    nL = inputImg.rows;
    nC = inputImg.cols * inputImg.channels();
  }
  //Step 2: Modelling each pixel with Gaussian
  outputImg = cv::Mat( inputImg.rows, inputImg.cols, CV_8UC1, cv::Scalar( 0 ) );
  while ( true )
  {
    double MahalDis;
    // Mahalanobis distance
    double sum = 0.0;
    bool close = false;
    int background;
    double tmpCovariance = 0.0;
    double var = 0.0;
    if ( !capture.read( inputImg ) )
    {
      break;
    }
    pixelPtr = pixelGaussianBuffer;
    for ( int i = 0; i < nL; i++ )
    {
      inputPtr = inputImg.ptr( i );
      outputPtr = outputImg.ptr( i );
      for ( int j = 0; j < nC; j += 3 )
      {
        sum = 0.0;
        close = false;
        background = 0;
        double rVal = *( inputPtr++ );
        double gVal = *( inputPtr++ );
        double bVal = *( inputPtr++ );
        head = pixelPtr->pixel_s;
        tail = pixelPtr->pixel_r;
        ptr = head;
        temp_ptr = NULL;
        if ( pixelPtr->numComponents > 4 )
        {
          Delete_gaussian( tail );
          pixelPtr->numComponents--;
        }
        for ( int k = 0; k < pixelPtr->numComponents; k++ )
        {
          double weight = ptr->weight;
          double mult = alpha / weight;
          weight = weight * alpha_bar + prune;
          if ( close == false )
          {
            double dR = rVal - ptr->mean[0];
            double dG = gVal - ptr->mean[1];
            double dB = bVal - ptr->mean[2];
            var = ptr->covariance;
            MahalDis = ( dR * dR + dG * dG + dB * dB );
            if ( ( sum < cfbar ) && ( MahalDis < 16.0 * var * var ) )
            {
              background = 255;
            }
            if ( MahalDis < 9.0 * var * var )
            {
              weight += alpha;
              close = true;
              ptr->mean[0] += mult * dR;
              ptr->mean[1] += mult * dG;
              ptr->mean[2] += mult * dB;
              tmpCovariance = var + mult * ( MahalDis - var );
              ptr->covariance = tmpCovariance < 5.0 ? 5.0 : ( tmpCovariance > 20.0 ? 20.0 : tmpCovariance );
              temp_ptr = ptr;
            }
          }
          if ( weight < -prune )
          {
            ptr = Delete_gaussian( ptr );
            weight = 0;
            pixelPtr->numComponents--;
          }
          else
          {
            sum += weight;
            ptr->weight = weight;
          }
          ptr = ptr->next;
        }
        if ( close == false )
        {
          ptr = new gaussian;
          ptr->weight = alpha;
          ptr->mean[0] = rVal;
          ptr->mean[1] = gVal;
          ptr->mean[2] = bVal;
          ptr->covariance = covariance0;
          ptr->next = NULL;
          ptr->prev = NULL;
          if ( head == NULL )
          {
            head = tail = NULL;
          }
          else
          {
            ptr->prev = tail;
            tail->next = ptr;
            tail = ptr;
          }
          temp_ptr = ptr;
          pixelPtr->numComponents++;
        }
        ptr = head;
        while ( ptr != NULL )
        {
          ptr->weight /= sum;
          ptr = ptr->next;
        }
        while ( temp_ptr != NULL && temp_ptr->prev != NULL )
        {
          if ( temp_ptr->weight <= temp_ptr->prev->weight )
          {
            break;
          }
          else
          {
            gaussian *nextptr, *previous;
            nextptr = temp_ptr->next;
            previous = temp_ptr->prev;
            if ( head == previous )
            {
              head = temp_ptr;
            }
            previous->next = nextptr;
            temp_ptr->prev = previous->prev;
            temp_ptr->next = previous;
            if ( previous->prev != NULL )
            {
              previous->prev->next = temp_ptr;
            }
            if ( nextptr != NULL )
            {
              nextptr->prev = previous;
            }
            else
            {
              tail = previous;
            }
            previous->prev = temp_ptr;
          }
          temp_ptr = temp_ptr->prev;
        }
        pixelPtr->pixel_s = head;
        pixelPtr->pixel_r = tail;
        *outputPtr++ = background;
        pixelPtr++;
      }
    }
    cv::imshow( "video", inputImg );
    cv::imshow( "GMM", outputImg );
    if ( cv::waitKey( 1 ) > 0 )
    {
      break;
    }
  }
  return EXIT_SUCCESS;
}
