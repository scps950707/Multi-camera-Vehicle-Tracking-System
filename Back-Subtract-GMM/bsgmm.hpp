#ifndef BSGMM_HPP
#define BSGMM_HPP

#include <opencv2/core/core.hpp>

/**
 *\file bsgmm.hpp
 *\brief Background Subtraction using Gaussian Mixture Models
 *
 */

const double alpha = 0.002;
/*! \var const double alpha
 * \brief Alpha's value will effect the speed of update.
 *
 * If the object be in static for approximately
 * log(1-cf) / log(1-alpha) frames,it will be considered
 * to be in the background
 *
 * the paper set alpha = 0.001 the result is 105 frames
 *
 * but here I set alpha = 0.002 the result is 52 frames
 *
 */
const double cT = 0.05;
const double defaultVariance = 11.0;
const double minVariance = 5.0;
const double maxVariance = 17.0;
const double cf = 0.1;
const double cfbar = 1.0 - cf;
const double prune = -alpha * cT;
const double alpha_bar = 1.0 - alpha;
const double BGSigma = 4 * 4;
const double closeSigma = 3 * 3;
const int defaultGMMCount = 4;

/**
 *  \brief enumeration for color
 */

enum Color
{
  BLACK = 0,
  WHITE = 255
};

/**
 * \brief Gaussian Mixture Models
 */

typedef struct
{
  /**
   * \brief stores RGB values from frames
   */
  double mean[3];
  /**
   * \brief modeling the distribution off RGB values
   */
  double variance;
  /**
   * \brief use to balance different GMM models
   */
  double weight;
} gaussian, *gaussianPtr;

/**
 * \brief Store GMM models for each model
 *
 * 4 GMM for each pixel is usually enough.
 *
 * We declare the array size plus one for implementation
 *
 * because sometimes number of GMM will bigger than 4 and will be resized to 4
 */

typedef struct
{
  /**
   * \brief Store GMM models
   */
  gaussian arr[defaultGMMCount + 1];
  /**
   * \brief Number of GMM of current pixel
   */
  int GMMCount;
} NODE, *NODEPTR;

class BackgroundSubtractorGMM
{
public:
  BackgroundSubtractorGMM( int frameHeight, int frameWidth );
  void initFirstFrame( uchar *inputPtr );
  void updateFrame( uchar *inputPtr, uchar *outputPtr );
  void freeMem();

private:
  NODE Create_Node();
  gaussian Create_gaussian( double r, double g, double b, double variance, double weight );
  NODEPTR pixelGMMBuffer, currentPixel;
  int frameHeight, frameWidth;
};

#endif
