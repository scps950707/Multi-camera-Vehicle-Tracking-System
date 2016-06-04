#ifndef BSGMM_HPP
#define BSGMM_HPP

#include <opencv2/core/core.hpp>

/**
 *\file bsgmm.hpp
 *\brief Background Subtraction using Gaussian Mixture Models
 *
 * Usage:
 *
 *
 * @code
 *
 * #include "bsgmm.hpp"
 *
 * cv::Mat inputImg, outputImg;
 *
 * BackgroundSubtractorGMM bsgmm(  inputImg.rows, inputImg.cols );
 *
 * bsgmm.initFirstFrame( inputImg.ptr() );
 *
 * And keep update frames using loop
 *
 * bsgmm.updateFrame( inputImg.ptr(), outputImg.ptr() );
 *
 * When works are done, release the resources
 *
 * bsgmm.freeMem();
 *
 * @endcode
 */

const double alpha = 0.002;
/**
 * \var alpha
 * \brief Alpha's value will effect the speed of update.
 *
 * If the object be in static for approximately
 * log(1-cf) / log(1-alpha) frames,it will be considered
 * to be in the background
 *
 * alpha = 0.001 the result is 105 frames
 *
 * alpha = 0.002 the result is 52 frames
 *
 * alpha = 0.003 the result is 35 frames
 *
 * which value of alpha depends on the fps of the video
 */
const double cT = 0.05;
/**
 * \var cT
 * \brief Number of samples needed to accept that a component actually exists
 */
const double defaultVariance = 11.0;
/**
 * \var defaultVariance
 * \brief default variance for GMM models
 */
const double minVariance = 5.0;
/**
 * \var minVariance
 * \brief min variance for GMM models
 *
 * From paper : in [5, 11] the pixel value distribution over time is modelled as an autoregressive process
 */
const double maxVariance = 17.0;
/**
 * \var maxVariance
 * \brief max variance for GMM models
 */
const double cf = 0.1;
/**
 * \var cf
 * \brief cf is a measure of the maximum portion of the data
 * that can belong to foreground objects without influencing
 * the background model
 */
const double cfbar = 1.0 - cf;
/**
 * \var cfbar
 * \brief The threshold that effect pixel to be part of background
 */
const double prune = -alpha * cT;
/**
 * \var prune
 * \brief A value to decide whether a GMM model should be removed
 */
const double alpha_bar = 1.0 - alpha;
/**
 * \var alpha_bar
 * \brief alpha_bar
 */
const double BGSigma = 4 * 4;
/**
 * \var BGSigma
 * \brief A threshold that effect pixel to be part of background
 *
 * In the paper : If Mahalanobis distance is smaller than three standard deviations
 * it can be part of the background, but here I pick 4
 */
const double closeSigma = 3 * 3;
/**
 * \var closeSigma
 * \brief A threshold that decide to add a new GMM model to pixel
 *
 * In the paper : If Mahalanobis distance is bigger than three standard deviations
 * add new GMM model to that pixel
 */
const int defaultGMMCount = 4;
/**
 * \var defaultGMMCount
 * \brief 4 GMMCount is enough to this paper's algorithm
 */

/**
 * \enum Color
 * \brief enumeration for color
 */

enum Color
{
  BLACK = 0,
  WHITE = 255
};

/**
 * \struct
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
 * \struct
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

/**
 * \class BackgroundSubtractorGMM
 * \brief The interface to use BackgroundSubtractorGMM
 */
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
