#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/features2d/features2d.hpp>
#include <stdlib.h>
#include "bsgmm.hpp"
#include <vector>
using namespace std;
NODEPTR pixelGaussianBuffer, pixelPtr;
NODE Create_Node( double r, double g, double b, double covariance, double weight )
{
  NODE tmp;
  tmp.numComponents = 1;
  tmp.pixelCompoListTail = tmp.pixelCompoListHead = Create_gaussian( r, g, b, covariance, weight );
  return tmp;
}
gaussianPtr Create_gaussian( double r, double g, double b, double covariance, double weight )
{
  gaussianPtr ptr = new gaussian;
  if ( ptr != NULL )
  {
    ptr->mean[0] = r;
    ptr->mean[1] = g;
    ptr->mean[2] = b;
    ptr->covariance = covariance;
    ptr->weight = weight;
    ptr->next = NULL;
    ptr->prev = NULL;
  }
  return ptr;
}

void insertBack( gaussianPtr *head, gaussianPtr *tail, gaussianPtr newPtr )
{
  if ( *head == NULL )
  {
    *head = *tail = newPtr;
  }
  else
  {
    newPtr->prev = *tail;
    ( *tail )->next = newPtr;
    *tail = newPtr;
  }
}

gaussianPtr popBack( gaussianPtr *head, gaussianPtr *tail, gaussianPtr target )
{
  gaussianPtr previous = target->prev;
  gaussianPtr nextptr = target->next;
  if ( head != NULL )
  {
    if ( target == *head && target == *tail )
    {
      *head = *tail = NULL;
      delete target;
    }
    else if ( target == *head )
    {
      nextptr->prev = NULL;
      *head = nextptr;
      delete target;
      target = *head;
    }
    else if ( target == *tail )
    {
      previous->next = NULL;
      *tail = previous;
      delete target;
      target = *tail;
    }
    else
    {
      previous->next = nextptr;
      nextptr->prev = previous;
      delete target;
      target = nextptr;
    }
  }
  else
  {
    std::cout << "Underflow........";
    exit( EXIT_FAILURE );
  }
  return target;
}

void swapNode( gaussianPtr *head, gaussianPtr *tail, gaussianPtr p1, gaussianPtr p2 )
{
  gaussianPtr nextptr = p2->next;
  if ( *head == p1 )
  {
    *head = p2;
  }
  p1->next = nextptr;
  p2->prev = p1->prev;
  p2->next = p1;
  if ( p1->prev != NULL )
  {
    p1->prev->next = p2;
  }
  if ( nextptr != NULL )
  {
    nextptr->prev = p1;
  }
  else
  {
    *tail = p1;
  }
  p1->prev = p2;
}

int main( int argc, char *argv[] )
{
  cv::Mat inputImg, outputImg;
  cv::VideoCapture capture( argv[1] );
  if ( !capture.read( inputImg ) )
  {
    std::cout << " Can't recieve input from source ";
    exit( EXIT_FAILURE );
  }
  cv::cvtColor( inputImg, inputImg, CV_BGR2YCrCb );
  outputImg = cv::Mat( inputImg.rows, inputImg.cols, CV_8U, cv::Scalar( 0 ) );
  uchar *inputPtr;
  uchar *outputPtr;
  pixelGaussianBuffer = new NODE[inputImg.rows * inputImg.cols];
  for ( int i = 0; i < inputImg.rows; i++ )
  {
    inputPtr = inputImg.ptr( i );
    for ( int j = 0; j < inputImg.cols; j++ )
    {
      pixelGaussianBuffer[i * inputImg.cols + j] = Create_Node( *inputPtr, *( inputPtr + 1 ), *( inputPtr + 2 ), defaultCovariance, alpha );
      pixelGaussianBuffer[i * inputImg.cols + j].pixelCompoListHead->weight = 1.0;
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
        gaussianPtr head = pixelPtr->pixelCompoListHead;
        gaussianPtr tail = pixelPtr->pixelCompoListTail;
        gaussianPtr ptr = pixelPtr->pixelCompoListHead;
        gaussianPtr temp_ptr = NULL;
        if ( pixelPtr->numComponents > 4 )
        {
          popBack( &head, &tail, tail );
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
            ptr = popBack( &head, &tail, ptr );
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
          ptr = Create_gaussian( rVal, gVal, bVal, defaultCovariance, alpha );
          insertBack( &head, &tail, ptr );
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
            swapNode( &head, &tail, temp_ptr->prev, temp_ptr );
          }
          temp_ptr = temp_ptr->prev;
        }
        pixelPtr->pixelCompoListHead = head;
        pixelPtr->pixelCompoListTail = tail;
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
