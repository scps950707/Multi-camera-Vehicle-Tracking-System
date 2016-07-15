#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "bsgmm.hpp"
using namespace std;

int main( int argc, char *argv[] )
{
#ifdef AVIOUTPUT
    if ( argc != 3 )
    {
        cout << "usage: ./excute [input] [output]" << endl;
    }
#else
    if ( argc != 2 )
    {
        cout << "usage: ./excute [input]" << endl;
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
    outputImg = cv::Mat( inputImg.rows, inputImg.cols, CV_8U, cv::Scalar( 0 ) );
#ifdef AVIOUTPUT
    cv::VideoWriter writer;
    writer.open( argv[2], CV_FOURCC( 'D', 'I', 'V', 'X' ), 30,
                 cv::Size( capture.get( CV_CAP_PROP_FRAME_WIDTH ), capture.get( CV_CAP_PROP_FRAME_HEIGHT ) ) );
#endif
    BackgroundSubtractorGMM bsgmm(  inputImg.rows, inputImg.cols );
    bsgmm.initFirstFrame( inputImg.ptr() );
    while ( capture.read( inputImg ) )
    {
        bsgmm.updateFrame( inputImg.ptr(), outputImg.ptr() );
#ifdef AVIOUTPUT
        cv::Mat frame;
        cv::cvtColor( outputImg, frame, CV_GRAY2RGB );
        writer << frame;
#else
        cv::imshow( "video", inputImg );
        cv::imshow( "GMM", outputImg );
#endif
        if ( cv::waitKey( 1 ) > 0 )
        {
            break;
        }
    }
    bsgmm.freeMem();
    return EXIT_SUCCESS;
}
