#include "header.hpp"
#include "bsgmm.hpp"
#include "rect.hpp"
#include "multiTracker.hpp"
#include "avi.hpp"

int main( int argc, char *argv[] )
{
    /* color array for drawing {{{*/
    array<cv::Scalar, 20> colors
    {
        {
            cv::Scalar( 113, 58, 83 ), cv::Scalar( 216, 132, 97 ), cv::Scalar( 216, 132, 97 ), cv::Scalar( 183, 197, 80 ),
            cv::Scalar( 101, 244, 240 ), cv::Scalar( 242, 223, 123 ), cv::Scalar( 239, 247, 178 ), cv::Scalar( 246, 247, 239 ),
            cv::Scalar( 224, 214, 247 ), cv::Scalar( 212, 181, 242 ), cv::Scalar( 229, 255, 248 ), cv::Scalar( 160, 214, 6 ),
            cv::Scalar( 170, 154, 27 ), cv::Scalar( 111, 71, 239 ), cv::Scalar( 61, 196, 255 ), cv::Scalar( 102, 102, 255 ),
            cv::Scalar( 102, 255, 204 ), cv::Scalar( 140, 46, 93 ), cv::Scalar( 182, 196, 46 ), cv::Scalar( 241, 232, 184 )
        }
    };
    /* }}} */

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
    MultiTracker tracker( 0.2, 0.5, 60.0, 10, 25 );
    /* }}} */

    /* {{{creat GMM Class object */

    BackgroundSubtractorGMM bsgmm( inputImg.rows, inputImg.cols );
    bsgmm.shadowBeBackground = true;

    /* }}} */

    while ( capture.read( inputImg ) )
    {
        /* DO GMM and morphologyEx {{{ */
        bsgmm.updateFrame( inputImg.ptr(), outputMask.ptr() );
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

        if ( boundRect.size() > 0 )
        {
            tracker.update( rect.getRectCentersFloat() );
            for ( size_t i = 0; i < tracker.tracks.size(); i++ )
            {
                if ( tracker.tracks[i].trackedPts.size() > 1 )
                {
                    for ( size_t j = 0; j < tracker.tracks[i].trackedPts.size() - 1; j++ )
                    {
                        line( inputImg, tracker.tracks[i].trackedPts[j], tracker.tracks[i].trackedPts[j + 1], colors[i], 2, CV_AA );
                    }
                    /* for ( size_t j = 0; j < tracker.tracks[i].trackedPts.size(); j++ ) */
                    /* { */
                    /* cv::circle( inputImg, tracker.tracks[i].trackedPts[j], 2, colors[i], CV_FILLED ); */
                    /* } */
                    putText( inputImg, to_string( tracker.tracks[i].trackId ),
                             cv::Point( tracker.tracks[i].trackedPts[0].x + 5,
                                        tracker.tracks[i].trackedPts[0].y + 5 ),
                             cv::FONT_HERSHEY_PLAIN, 1, RED_C3, 2 );
                }
            }
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
            cv::imshow( "Tracking DEMO", merge );
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
