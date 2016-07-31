#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <getopt.h>
#include "bsgmm.hpp"
#include "rect.hpp"
using namespace std;

int main( int argc, char *argv[] )
{

    // codes for control command line options

    bool aviOutput = false;
    bool maskOutput = false;
    int  options;
    string videoOutPath, maskOutPath, inputPath;
    if ( argc == 1 )
    {
        cout << "usage: ./excute -i [input] -v [output] -m [mask_output]" << endl;
        exit( EXIT_FAILURE );
    }
    struct option  long_opt[] =
    {
        {"input", required_argument, NULL, 'i'},
        {"mask", required_argument, NULL, 'm'},
        {"video", required_argument, NULL, 'v'},
        {NULL, 0, NULL, 0}
    };
    while ( ( options = getopt_long( argc, argv, "i:m:v:", long_opt, NULL ) ) != -1 )
    {
        switch  ( options )
        {
        case 'i':
            inputPath = string( optarg );
            break;
        case 'm':
            maskOutPath = string( optarg );
            maskOutput = true;
            break;
        case 'v':
            aviOutput = true;
            videoOutPath = string( optarg );
            break;
        }
    }

    // declare mat for input and mask

    cv::Mat inputImg, outputImg;
    cv::VideoCapture capture( inputPath );
    if ( !capture.read( inputImg ) )
    {
        cout << " Can't recieve input from source " << endl;
        exit( EXIT_FAILURE );
    }
    if ( inputImg.cols > 1800 && inputImg.rows > 900 )
    {
        cv::resize( inputImg, inputImg, cv::Size( inputImg.cols / 2, inputImg.rows / 2 ) );
    }
    outputImg = cv::Mat( inputImg.rows, inputImg.cols, CV_8UC1, cv::Scalar( 0 ) );

    //declare output stream

    cv::VideoWriter writer, writer2;
    if ( aviOutput )
    {
        writer.open( videoOutPath, CV_FOURCC( 'D', 'I', 'V', 'X' ), 30,
                     cv::Size( capture.get( CV_CAP_PROP_FRAME_WIDTH ), capture.get( CV_CAP_PROP_FRAME_HEIGHT ) ) );
    }
    if ( maskOutput )
    {
        writer2.open( maskOutPath, CV_FOURCC( 'D', 'I', 'V', 'X' ), 30,
                      cv::Size( capture.get( CV_CAP_PROP_FRAME_WIDTH ), capture.get( CV_CAP_PROP_FRAME_HEIGHT ) ) );
    }

    //creat GMM Class object

    BackgroundSubtractorGMM bsgmm(  inputImg.rows, inputImg.cols );
    bsgmm.shadowBeBackground = true;


    while ( capture.read( inputImg ) )
    {
        if ( inputImg.cols > 1800 && inputImg.rows > 900 )
        {
            cv::resize( inputImg, inputImg, cv::Size( inputImg.cols / 2, inputImg.rows / 2 ) );
        }
        cv::Mat inputBlur;
        cv::blur( inputImg, inputBlur, cv::Size( 4, 4 ) );
        bsgmm.updateFrame( inputBlur.ptr(), outputImg.ptr() );
        cv::Mat outputMorp;
        cv::morphologyEx( outputImg, outputMorp, CV_MOP_CLOSE, getStructuringElement( cv::MORPH_RECT, cv::Size( 5, 5 ), cv::Point( 3, 3 ) ) );

        // draw rect
        findRect rect;
        rect.findBoundingRect( inputImg, outputMorp );

        // print words on img for debug
        char str[20];
        sprintf( str, "Frame:%d", ( int )capture.get( CV_CAP_PROP_POS_FRAMES ) );
        putText( inputImg, str, cv::Point( 450, inputImg.rows - 20 ), cv::FONT_HERSHEY_PLAIN, 2,  cv::Scalar( 0, 0, 255 ), 2 );

        cv::imshow( "video", inputImg );
        cv::imshow( "GMM", outputImg );
        cv::imshow( "inputBlur", inputBlur );
        cv::imshow( "outputMorp", outputMorp );

        // write to avi
        if ( aviOutput )
        {
            writer << inputImg;
        }
        if ( maskOutput )
        {
            cv::Mat maskForAvi;
            cv::cvtColor( outputMorp, maskForAvi, CV_GRAY2RGB );
            //because our mask is single channel, we need to convert it to three channel to output avi
            sprintf( str, "Frame:%d", ( int )capture.get( CV_CAP_PROP_POS_FRAMES ) );
            putText( maskForAvi , str, cv::Point( 450, inputImg.rows - 20 ), cv::FONT_HERSHEY_PLAIN, 2,  cv::Scalar( 0, 0, 255 ), 2 );
            writer2 << maskForAvi;
        }

        // monitor keys to stop
        if ( cv::waitKey( 1 ) > 0 )
        {
            break;
        }
    }
    bsgmm.freeMem();
    return EXIT_SUCCESS;
}
