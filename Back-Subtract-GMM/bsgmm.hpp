#ifndef BSGMM_HPP
#define BSGMM_HPP

#include <opencv2/core/core.hpp>


/*
 * Usage:
 * #include "bsgmm.hpp"
 * cv::Mat inputImg, outputImg;
 * BackgroundSubtractorGMM bsgmm(  inputImg.rows, inputImg.cols );
 * And keep update frames using loop
 * bsgmm.updateFrame( inputImg.ptr(), outputImg.ptr() );
 * When works are done, release the resources
 * bsgmm.freeMem();
 */


enum Color
{
    BLACK = 0,
    WHITE = 255,
    GRAY  = 127
};

// Gaussian Mixture Models

typedef struct
{
    // stores RGB values from frames
    double R;
    double G;
    double B;
    // modeling the distribution off RGB values
    double variance;
    // use to balance different GMM models
    double weight;
} gaussian;

/*
 * Store GMM models for each model
 * 4 GMM for each pixel is usually enough.
 * We declare the array size plus one for implementation
 * because sometimes number of GMM will bigger than 4 and will be resized to 4
 */

typedef struct
{
    // Store GMM models
    gaussian arr[4];
    // Number of GMM of current pixel
    int GMMCount;
} PIXELGMM;

class BackgroundSubtractorGMM
{
public:
    BackgroundSubtractorGMM( int frameHeight, int frameWidth );
    void updateFrame( uchar *inputPtr, uchar *outputPtr );
    void freeMem();
    double alpha;
    /*
     * If the object be in static for approximately
     * log(1-cf) / log(1-alpha) frames,it will be considered
     * to be in the background
     * alpha = 0.001 the result is 105 frames
     * alpha = 0.002 the result is 52 frames
     * alpha = 0.003 the result is 35 frames
     * which value of alpha depends on the fps of the video
     */
    double alpha_bar;
    double cT;
    // Number of samples needed to accept that a component actually exists
    double defaultVariance;
    // default variance for GMM models
    double minVariance;
    /*
     * min variance for GMM models
     * From paper : in [5, 11] the pixel value distribution over time is modelled as an autoregressive process
     */
    double maxVariance;
    // max variance for GMM models
    double cf;
    /*
     * cf is a measure of the maximum portion of the data
     * that can belong to foreground objects without influencing
     * the background model
     */
    double cfbar;
    // The threshold that effect pixel to be part of background
    double prune;
    // A value to decide whether a GMM model should be removed
    double BGSigma;
    /*
     * A threshold that effect pixel to be part of background
     * In the paper : If Mahalanobis distance is smaller than three standard deviations
     * it can be part of the background, but here I pick 4
     */
    double closeSigma;
    /*
     * A threshold that decide to add a new GMM model to pixel
     * In the paper : If Mahalanobis distance is bigger than three standard deviations
     * add new GMM model to that pixel
     */
    int defaultGMMCount;
    // 4 GMMCount is enough to this paper's algorithm
    bool shadowDetection;
    double tau;
    bool removeForeground;
    bool shadowBeBackground;
    /*
     * Shadow threshold.
     * The shadow is detected if the pixel is a darker version of the background.
     * Tau is a threshold defining how much darker the shadow can be.
     * Tau= 0.5 means that if a pixel is more than twice darker then it is not shadow.
	 */


private:
    bool isBackGround( double red, double green, double blue, PIXELGMM *currentPixel );
    bool isShadow( double red, double green, double blue, PIXELGMM *currentPixel );
    PIXELGMM *pixelGMMBuffer;
    int frameHeight, frameWidth;
};

#endif
