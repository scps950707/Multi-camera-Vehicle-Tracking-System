#include "header.hpp"
#include "bsgmm.hpp"
#include "rect.hpp"
#include "Ctracker.hpp"

int main( int argc, char *argv[] )
{
    cv::RNG rng( 12345 );
    /* codes for control command line options {{{ */
    cv::Scalar colors[20];
    for ( int i = 0; i < 20; i++ )
    {
        colors[i] = cv::Scalar( rng.uniform( 0, 255 ), rng.uniform( 0, 255 ), rng.uniform( 0, 255 ) );
    }

    bool aviOutput = false;
    bool maskOutput = false;
    int fastforward = 0;
    int  options;
    string videoOutPath, maskOutPath, inputPath;

    if ( argc == 1 )
    {
        cout << "usage: ./BSGMM [options]" << endl;
        cout << "options:" << endl;
        cout << "-i [input video path]  (required)" << endl;
        cout << "-v [output video path]" << endl;
        cout << "-m [mask video output path]" << endl;
        cout << "-t [video start time (secs)]" << endl;
        exit( EXIT_FAILURE );
    }
    struct option  long_opt[] =
    {
        {"input", required_argument, NULL, 'i'},
        {"mask", required_argument, NULL, 'm'},
        {"video", required_argument, NULL, 'v'},
        {"time", required_argument, NULL, 't'},
        {NULL, 0, NULL, 0}
    };
    while ( ( options = getopt_long( argc, argv, "i:m:v:t:", long_opt, NULL ) ) != -1 )
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
        case 't':
            fastforward = atoi( optarg );
            break;
        }
    }

    /* }}} */

    /* {{{ declare mat for input and mask */

    cv::Mat inputImg, outputMask;
    cv::Size newSize( 800, 450 );
    cv::VideoCapture capture( inputPath );
    // perform fast foward
    capture.set( CV_CAP_PROP_POS_FRAMES, fastforward * FPS );
    if ( !capture.read( inputImg ) )
    {
        cout << " Can't recieve input from source " << endl;
        exit( EXIT_FAILURE );
    }
    cv::resize( inputImg, inputImg, newSize );
    outputMask = cv::Mat( inputImg.size(), CV_8UC1, BLACK_C1 );
    /* }}} */

    /* declare output stream{{{ */

    cv::VideoWriter writer, writer2;
    if ( aviOutput )
    {
        writer.open( videoOutPath, CV_FOURCC( 'D', 'I', 'V', 'X' ), FPS, newSize );
    }
    if ( maskOutput )
    {
        writer2.open( maskOutPath, CV_FOURCC( 'D', 'I', 'V', 'X' ), FPS, newSize );
    }
    /* }}} */

    /* {{{creat GMM Class object */

    BackgroundSubtractorGMM bsgmm(  inputImg.rows, inputImg.cols );
    bsgmm.shadowBeBackground = true;

    /* }}} */

    CTracker tracker( 0.2, 0.5, 60.0, 30, 25 );
    while ( capture.read( inputImg ) )
    {
        cv::resize( inputImg, inputImg, newSize );
        /* cv::Mat inputBlur; */
        /* cv::GaussianBlur( inputImg, inputBlur, cv::Size( 5, 5 ), 0, 0 ); */
        /* bsgmm.updateFrame( inputBlur.ptr(), outputMask.ptr() ); */
        bsgmm.updateFrame( inputImg.ptr(), outputMask.ptr() );
        cv::Mat outputMorp;
        cv::morphologyEx( outputMask, outputMorp, CV_MOP_CLOSE, getStructuringElement( cv::MORPH_RECT, cv::Size( 5, 5 ) ) );

        /* draw rect print words on img for debug {{{ */
        findRect rect;
        vector<cv::Rect> boundRect =  rect.findBoundingRect( inputImg, outputMorp );

        for ( unsigned int i = 0; i < boundRect.size(); i++ )
        {
            rectangle( inputImg, boundRect[i].tl(), boundRect[i].br(), RED_C3, 2 );
        }
        if ( boundRect.size() > 0 )
        {
            vector<cv::Point2f> pts;
            for ( unsigned int i = 0; i < boundRect.size(); i++ )
            {
                pts.push_back( cv::Point( boundRect[i].x + boundRect[i].width / 2, boundRect[i].y + boundRect[i].height / 2 ) );
            }
            tracker.Update( pts );

            for ( size_t i = 0; i < pts.size(); i++ )
            {
                circle( inputImg, pts[i], 3, GREEN_C3, 1, CV_AA );
            }

            for ( size_t i = 0; i < tracker.tracks.size(); i++ )
            {
                if ( tracker.tracks[i]->trace.size() > 1 )
                {
                    /* for ( size_t j = 0; j < tracker.tracks[i]->trace.size() - 1; j++ ) */
                    /* { */
                    /*     line( inputImg, tracker.tracks[i]->trace[j], tracker.tracks[i]->trace[j + 1], colors[i], 2, CV_AA ); */
                    /* } */
                    for ( size_t j = 0; j < tracker.tracks[i]->trace.size(); j++ )
                    {
                        cv::circle( inputImg, tracker.tracks[i]->trace[j], 2, colors[i], CV_FILLED );
                    }
                }
            }
        }

        string str = "Count:" + to_string( boundRect.size() ) + " Frame:" + to_string( ( int )capture.get( CV_CAP_PROP_POS_FRAMES ) ) + "time:" + to_string( ( int )( capture.get( CV_CAP_PROP_POS_FRAMES ) / FPS ) );
        putText( inputImg, str, cv::Point( 300, inputImg.rows - 20 ), cv::FONT_HERSHEY_PLAIN, 2,  RED_C3, 2 );
        /* }}} */

        cv::imshow( "video", inputImg );
        cv::imshow( "GMM", outputMask );
        /* cv::imshow( "inputBlur", inputBlur ); */
        cv::imshow( "outputMorp", outputMorp );

        /* write to avi{{{ */
        if ( aviOutput )
        {
            writer << inputImg;
        }
        if ( maskOutput )
        {
            cv::Mat maskForAvi;
            cv::cvtColor( outputMorp, maskForAvi, CV_GRAY2RGB );
            //because our mask is single channel, we need to convert it to three channel to output avi
            putText( maskForAvi , str, cv::Point( 300, inputImg.rows - 20 ), cv::FONT_HERSHEY_PLAIN, 2,  RED_C3, 2 );
            writer2 << maskForAvi;
        }
        /* }}} */

        /* monitor keys to stop{{{ */
        if ( cv::waitKey( 1 ) > 0 )
        {
            break;
        }
        /* }}} */
    }
    bsgmm.freeMem();
    return EXIT_SUCCESS;
}
