#include <opencv2/core/core.hpp>
#include "bsgmm.hpp"
using namespace std;

/**
 *\file bsgmm.cpp
 *\brief Implementation of Background Subtraction using Gaussian Mixture Models
 *
 */

BackgroundSubtractorGMM::BackgroundSubtractorGMM( int frameHeight, int frameWidth ) : frameHeight( frameHeight ), frameWidth( frameWidth )
{
  pixelGMMBuffer = new NODE[frameHeight * frameWidth];
}

void BackgroundSubtractorGMM::initFirstFrame( uchar *inputPtr )
{
  for ( int i = 0; i < frameHeight; i++ )
  {
    for ( int j = 0; j < frameWidth; j++ )
    {
      pixelGMMBuffer[i * frameWidth + j] = Create_Node();
      pixelGMMBuffer[i * frameWidth + j].arr[0] =
        Create_gaussian( *inputPtr, *( inputPtr + 1 ), *( inputPtr + 2 ), defaultVariance, 1.0 );
    }
  }
}

void BackgroundSubtractorGMM::updateFrame( uchar *inputPtr, uchar *outputPtr )
{
  currentPixel = pixelGMMBuffer;
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
}

void BackgroundSubtractorGMM::freeMem()
{
  delete[] pixelGMMBuffer;
}

NODE BackgroundSubtractorGMM::Create_Node()
{
  NODE tmp;
  tmp.GMMCount = 1;
  return tmp;
}

gaussian BackgroundSubtractorGMM::Create_gaussian( double r, double g, double b, double variance, double weight )
{
  gaussian tmp;
  tmp.mean[0] = r;
  tmp.mean[1] = g;
  tmp.mean[2] = b;
  tmp.variance = variance;
  tmp.weight = weight;
  return tmp;
}
