#include "header.hpp"
#include "rect.hpp"
#include "avi.hpp"

int main( int argc, char *argv[] )
{

    /* codes for control command line options {{{ */

    bool outputAvi = false;
    bool imshow = true;
    int fastforward = 0;
    int options;
    string videoOutPath, inputPath;

    if ( argc == 1 )
    {
        cout << "usage: ./gmm [options]" << endl;
        cout << "options:" << endl;
        cout << "-i [input video path] (required)" << endl;
        cout << "[-n] [--noImshow] (optional)" << endl;
        cout << "-o [output video path] (optional)" << endl;
        cout << "-t [video start time (secs)] (optional)" << endl;
        exit( EXIT_FAILURE );
    }
    struct option long_opt[] =
    {
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {"noImshow", required_argument, NULL, 'n'},
        {"time", required_argument, NULL, 't'},
        {NULL, 0, NULL, 0}
    };
    while ( ( options = getopt_long( argc, argv, "i:no:t:", long_opt, NULL ) ) != -1 )
    {
        switch ( options )
        {
        case 'i':
            inputPath = string( optarg );
            break;
        case 'n':
            imshow = false;
            break;
        case 'o':
            outputAvi = true;
            videoOutPath = string( optarg );
            break;
        case 't':
            fastforward = atoi( optarg );
            break;
        }
    }

    /* }}} */

    /* {{{ global variable declaration */

    cv::Mat inputImg;
    cv::VideoCapture capture( inputPath );
    // perform fast foward
    capture.set( CV_CAP_PROP_POS_FRAMES, fastforward * FPS );
    if ( !capture.read( inputImg ) )
    {
        cout << " Can't recieve input from source " << endl;
        exit( EXIT_FAILURE );
    }
    cv::Mat outputMask = cv::Mat( inputImg.size(), CV_8UC1, BLACK_C1 );
    cv::Mat merge( inputImg.rows * 2, inputImg.cols, inputImg.type() );
    aviWriter aw;
    if ( outputAvi )
    {
        aw = aviWriter( videoOutPath, FPS, merge.size() );
    }
    /* }}} */

    /* {{{creat GMM Class object */

    cv::BackgroundSubtractorMOG2 mog2;
    /* mog2.setInt( "nmixtures", 4 ); */
    /* mog2.setDouble( "backgroundRatio", 0.9 ); */
    /* mog2.setDouble( "varThresholdGen", 9 ); */
    /* mog2.setDouble( "fVarInit", 11 ); */
    /* mog2.setDouble( "fVarMin", 4 ); */
    /* mog2.setDouble( "fVarMax", 55 ); */
    /* mog2.setDouble( "fCT", 0.05 ); */
    /* mog2.setDouble( "fTau", 0.5 ); */
    mog2.setBool( "detectShadows", false );

    /* }}} */

    while ( capture.read( inputImg ) )
    {
        /* DO GMM and morphologyEx {{{ */
        mog2( inputImg, outputMask );
        cv::Mat outputMorp;
        cv::morphologyEx( outputMask, outputMorp, CV_MOP_CLOSE, getStructuringElement( cv::MORPH_RECT, cv::Size( 5, 5 ) ) );
        /* }}} */

        /* draw rect print words on img for debug {{{ */
        findRect rect;
        rect.update( inputImg, outputMorp );
        vector<cv::Rect> boundRect = rect.getRects();

        for ( unsigned int i = 0; i < boundRect.size(); i++ )
        {
            rectangle( inputImg, boundRect[i].tl(), boundRect[i].br(), RED_C3, 2 );
        }

        string str = "Count:" + to_string( boundRect.size() ) + " Frame:" + to_string( ( int )capture.get( CV_CAP_PROP_POS_FRAMES ) ) + "time:" + to_string( ( int )( capture.get( CV_CAP_PROP_POS_FRAMES ) / FPS ) );
        putText( inputImg, str, cv::Point( 300, inputImg.rows - 20 ), cv::FONT_HERSHEY_PLAIN, 2, RED_C3, 2 );
        /* }}} */

        /* Merge results {{{ */
        merge.setTo( 0 );
        inputImg.copyTo( merge( cv::Range( 0, inputImg.rows ), cv::Range( 0, inputImg.cols ) ) );
        cv::Mat maskBGR( outputMask.size(), outputMask.type() );
        cv::cvtColor( outputMask, maskBGR, CV_GRAY2BGR );
        maskBGR.copyTo( merge( cv::Range( inputImg.rows, inputImg.rows + maskBGR.rows ), cv::Range( 0, maskBGR.cols ) ) );
        /* }}} */

        /* Code for imshow {{{*/
        if ( imshow )
        {
            cv::imshow( "gmmOpencv", merge );
        }
        /* }}} */

        /* write to avi{{{ */
        if ( outputAvi )
        {
            aw << merge;
        }
        /* }}} */

        /* monitor keys to stop{{{ */
        if ( cv::waitKey( 1 ) > 0 )
        {
            break;
        }
        /* }}} */
    }
    return EXIT_SUCCESS;
}
