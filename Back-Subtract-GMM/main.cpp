#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "bsgmm.hpp"
using namespace std;

int main( int argc, char *argv[] )
{
#ifdef AVI
    if ( argc != 3 )
    {
        cout << "usage: ./excute [input] [output]" << endl;
        exit( EXIT_FAILURE );
    }
#else
    if ( argc != 2 )
    {
        cout << "usage: ./excute [input]" << endl;
        exit( EXIT_FAILURE );
    }
#endif
    cv::Mat inputImg, outputImg;
    cv::VideoCapture capture( argv[1] );
    if ( !capture.read( inputImg ) )
    {
        cout << " Can't recieve input from source " << endl;
        exit( EXIT_FAILURE );
    }
    cv::cvtColor( inputImg, inputImg, CV_BGR2YCrCb );
    outputImg = cv::Mat( inputImg.rows, inputImg.cols, CV_8UC1, cv::Scalar( 0 ) );
#ifdef AVI
    cv::VideoWriter writer;
    writer.open( argv[2], CV_FOURCC( 'D', 'I', 'V', 'X' ), 30,
                 cv::Size( capture.get( CV_CAP_PROP_FRAME_WIDTH ), capture.get( CV_CAP_PROP_FRAME_HEIGHT ) ) );
#endif
    BackgroundSubtractorGMM bsgmm(  inputImg.rows, inputImg.cols );
    bsgmm.initFirstFrame( inputImg.ptr() );
    while ( capture.read( inputImg ) )
    {
        bsgmm.updateFrame( inputImg.ptr(), outputImg.ptr() );
        cv::imshow( "video", inputImg );
        cv::imshow( "GMM", outputImg );
#ifdef AVI
        writer << inputImg;

        /* these codes outputs GMM mask result */
        /* cv::Mat maskForAvi; */
        /* cv::cvtColor(outputImg,maskForAvi,CV_GRAY2RGB); */
        /* writer<<maskForAvi; */
#endif
        if ( cv::waitKey( 1 ) > 0 )
        {
            break;
        }
    }
    bsgmm.freeMem();
    return EXIT_SUCCESS;
}
