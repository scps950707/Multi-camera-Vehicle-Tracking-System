#ifndef BSGMM_HPP
#define BSGMM_HPP

#include <opencv2/core/core.hpp>


/*
 * Usage:
 * #include "bsgmm.hpp"
 * cv::Mat inputImg, outputImg;
 * BackgroundSubtractorGMM bsgmm(  inputImg.rows, inputImg.cols );
 * bsgmm.initFirstFrame( inputImg.ptr() );
 * And keep update frames using loop
 * bsgmm.updateFrame( inputImg.ptr(), outputImg.ptr() );
 * When works are done, release the resources
 * bsgmm.freeMem();
 */

const double alpha = 0.002;
/*
 * If the object be in static for approximately
 * log(1-cf) / log(1-alpha) frames,it will be considered
 * to be in the background
 * alpha = 0.001 the result is 105 frames
 * alpha = 0.002 the result is 52 frames
 * alpha = 0.003 the result is 35 frames
 * which value of alpha depends on the fps of the video
 */
const double cT = 0.05;
// Number of samples needed to accept that a component actually exists
const double defaultVariance = 11.0;
// default variance for GMM models
const double minVariance = 5.0;
/*
 * min variance for GMM models
 * From paper : in [5, 11] the pixel value distribution over time is modelled as an autoregressive process
 */
const double maxVariance = 17.0;
// max variance for GMM models
const double cf = 0.1;
/*
 * cf is a measure of the maximum portion of the data
 * that can belong to foreground objects without influencing
 * the background model
 */
const double cfbar = 1.0 - cf;
// The threshold that effect pixel to be part of background
const double prune = -alpha * cT;
// A value to decide whether a GMM model should be removed
const double alpha_bar = 1.0 - alpha;
const double BGSigma = 4 * 4;
/*
 * A threshold that effect pixel to be part of background
 * In the paper : If Mahalanobis distance is smaller than three standard deviations
 * it can be part of the background, but here I pick 4
 */
const double closeSigma = 3 * 3;
/*
 * A threshold that decide to add a new GMM model to pixel
 * In the paper : If Mahalanobis distance is bigger than three standard deviations
 * add new GMM model to that pixel
 */
const int defaultGMMCount = 4;
// 4 GMMCount is enough to this paper's algorithm

enum Color
{
    BLACK = 0,
    WHITE = 255
};

// Gaussian Mixture Models

typedef struct
{
    // stores RGB values from frames
    double mean[3];
    // modeling the distribution off RGB values
    double variance;
    // use to balance different GMM models
    double weight;
} gaussian, *gaussianPtr;

/*
 * Store GMM models for each model
 * 4 GMM for each pixel is usually enough.
 * We declare the array size plus one for implementation
 * because sometimes number of GMM will bigger than 4 and will be resized to 4
 */

typedef struct
{
    // Store GMM models
    gaussian arr[defaultGMMCount + 1];
    // Number of GMM of current pixel
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
