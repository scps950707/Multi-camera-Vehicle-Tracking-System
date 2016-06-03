#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include "bsgmm.hpp"
using namespace std;
NODEPTR pixelGMMBuffer, currentPixel;
static int frameHeight, frameWidth;

NODE Create_Node()
{
  NODE tmp;
  tmp.GMMCount = 1;
  return tmp;
}

gaussian Create_gaussian( double r, double g, double b, double variance, double weight )
{
  gaussian tmp;
  tmp.mean[0] = r;
  tmp.mean[1] = g;
  tmp.mean[2] = b;
  tmp.variance = variance;
  tmp.weight = weight;
  return tmp;
}

void freeMem()
{
  delete[] pixelGMMBuffer;
}

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
  frameWidth = inputImg.rows;
  frameHeight = inputImg.cols;
  uchar *inputPtr;
  uchar *outputPtr;
  pixelGMMBuffer = new NODE[inputImg.rows * inputImg.cols];
  for ( int i = 0; i < inputImg.rows; i++ )
  {
    inputPtr = inputImg.ptr( i );
    for ( int j = 0; j < inputImg.cols; j++ )
    {
      pixelGMMBuffer[i * inputImg.cols + j] = Create_Node();
      pixelGMMBuffer[i * inputImg.cols + j].arr[0] =
        Create_gaussian( *inputPtr, *( inputPtr + 1 ), *( inputPtr + 2 ), defaultVariance, 1.0 );
    }
  }
  capture.read( inputImg );
  outputImg = cv::Mat( inputImg.rows, inputImg.cols, CV_8UC1, cv::Scalar( 0 ) );
  while ( capture.read( inputImg ) )
  {
    currentPixel = pixelGMMBuffer;
    inputPtr = inputImg.ptr( 0 );
    outputPtr = outputImg.ptr( 0 );
    for ( int i = 0; i < frameWidth * frameHeight; i++ )
    {
      double MahalDis;
      double newVariance = 0.0;
      double var = 0.0;
      double totalWeight = 0.0;
      double rVal = *( inputPtr++ );
      double gVal = *( inputPtr++ );
      double bVal = *( inputPtr++ );
      bool hitGMM = false;
      int sortIndex;
      int background = BLACK;
      if ( currentPixel->GMMCount > defaultGMMCount )
      {
        currentPixel->GMMCount--;
      }
      for ( int GMMIndex = 0; GMMIndex < currentPixel->GMMCount; GMMIndex++ )
      {
        double weight = currentPixel->arr[GMMIndex].weight;
        weight = weight * alpha_bar + prune;
        if ( hitGMM == false )
        {
          double dR = rVal - currentPixel->arr[GMMIndex].mean[0];
          double dG = gVal - currentPixel->arr[GMMIndex].mean[1];
          double dB = bVal - currentPixel->arr[GMMIndex].mean[2];
          var = currentPixel->arr[GMMIndex].variance;
          MahalDis = ( dR * dR + dG * dG + dB * dB );
          if ( ( totalWeight < cfbar ) && ( MahalDis < BGSigma * var * var ) )
          {
            background = WHITE;
          }
          if ( MahalDis < closeSigma * var * var )
          {
            weight += alpha;
            hitGMM = true;
            double mult = alpha / weight;
            currentPixel->arr[GMMIndex].mean[0] += mult * dR;
            currentPixel->arr[GMMIndex].mean[1] += mult * dG;
            currentPixel->arr[GMMIndex].mean[2] += mult * dB;
            newVariance = var + mult * ( MahalDis - var );
            currentPixel->arr[GMMIndex].variance = newVariance < minVariance ? minVariance
                                                   : ( newVariance > maxVariance ? maxVariance : newVariance );
            sortIndex = GMMIndex;
          }
        }
        if ( weight < -prune )
        {
          currentPixel->GMMCount--;
        }
        else
        {
          totalWeight += weight;
          currentPixel->arr[GMMIndex].weight = weight;
        }
      }
      if ( hitGMM == false )
      {
        sortIndex = currentPixel->GMMCount;
        currentPixel->arr[currentPixel->GMMCount++] = Create_gaussian( rVal, gVal, bVal, defaultVariance, alpha );
      }
      for ( int GMMIndex = 0; GMMIndex < currentPixel->GMMCount; GMMIndex++ )
      {
        currentPixel->arr[GMMIndex].weight /= totalWeight;
      }
      for ( int GMMIndex = sortIndex; GMMIndex > 0; GMMIndex-- )
      {
        if ( currentPixel->arr[GMMIndex].weight <= currentPixel->arr[GMMIndex - 1].weight )
        {
          break;
        }
        else
        {
          swap( currentPixel->arr[GMMIndex], currentPixel->arr[GMMIndex - 1] );
        }
      }
      *outputPtr++ = background;
      currentPixel++;
    }
    cv::imshow( "video", inputImg );
    cv::imshow( "GMM", outputImg );
    if ( cv::waitKey( 1 ) > 0 )
    {
      break;
    }
  }
  freeMem();
  return EXIT_SUCCESS;
}
