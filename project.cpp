#include "header.hpp"
#include "bsgmm.hpp"
#include "rect.hpp"
#include "ptrans.hpp"
#include "gnuplot-iostream.h"
#include "avi.hpp"
#include "Ctracker.hpp"

int main( int argc, char *argv[] )
{
    /* {{{ global variable set by commad line parameters */
    int fastforward = 0;
    int  options;
    string videoOutPath, maskOutPath, inputPath711, inputPathKymco, outputPath;
    bool outputAvi = false;
    bool Imshow = true;
    /* }}} */

    // codes for control command line options {{{


    if ( argc == 1 )
    {
        cout << "./usage [options]" << endl;
        cout << "options:" << endl;
        cout << "-i [input 711 video path] (required)" << endl;
        cout << "-j [input kymco video path] (required)" << endl;
        cout << "[-n] [--noImshow] (optional)" << endl;
        cout << "-o [output video path] (optional)" << endl;
        cout << "-t [video start time (secs)] (optional)" << endl;
        exit( EXIT_FAILURE );
    }
    struct option  long_opt[] =
    {
        {"input", required_argument, NULL, 'i'},
        {"input2", required_argument, NULL, 'j'},
        {"noImshow", required_argument, NULL, 'n'},
        {"output", required_argument, NULL, 'o'},
        {"time", required_argument, NULL, 't'},
        {NULL, 0, NULL, 0}
    };
    while ( ( options = getopt_long( argc, argv, "i:j:no:t:", long_opt, NULL ) ) != -1 )
    {
        switch  ( options )
        {
        case 'i':
            inputPath711 = string( optarg );
            break;
        case 'j':
            inputPathKymco = string( optarg );
            break;
        case 'n':
            Imshow = false;
            break;
        case 'o':
            outputPath = string( optarg );
            outputAvi = true;
            break;
        case 't':
            fastforward = atoi( optarg );
            break;
        }
    }

    // }}}

    // {{{ global variable declaration

    cv::Mat inputImg711, outputMask711;
    cv::Mat inputImgKymco, outputMaskKymco;
    cv::Size newSize( 800, 450 );
    cv::VideoCapture capture711( inputPath711 );
    cv::VideoCapture captureKymco( inputPathKymco );
    // perform fast foward
    capture711.set( CV_CAP_PROP_POS_FRAMES, fastforward * FPS );
    if ( !capture711.read( inputImg711 ) )
    {
        cout << " Can't recieve input from source " << endl;
        exit( EXIT_FAILURE );
    }
    captureKymco.set( CV_CAP_PROP_POS_FRAMES, fastforward * FPS );
    if ( !captureKymco.read( inputImgKymco ) )
    {
        cout << " Can't recieve input from source " << endl;
        exit( EXIT_FAILURE );
    }
    /* cv::resize( inputImg711, inputImg711, newSize ); */
    /* cv::resize( inputImgKymco, inputImgKymco, newSize ); */
    outputMask711 = cv::Mat( inputImg711.size(), CV_8UC1, BLACK_C1 );
    outputMaskKymco = cv::Mat( inputImgKymco.size(), CV_8UC1, BLACK_C1 );
    cv::Mat originRoadMap( cv::Size( 600, 600 ), inputImg711.type(), GRAY_C3 );
    cv::Mat merge = cv::Mat::zeros( inputImg711.rows * 2 + 5, inputImg711.cols + originRoadMap.cols + 5, inputImg711.type() );
    aviWriter aw;
    if ( outputAvi )
    {
        aw = aviWriter( outputPath, FPS, merge.size() );
    }
    // }}}

    // {{{creat rotation matrix

    perspectiveTransform ptrans711;
    ptrans711.setSrcPts( cv::Point2f( 330, 95 ), cv::Point2f( 0, 180 ), cv::Point2f( 745, 410 ), cv::Point2f( 665, 145 ) );
    ptrans711.setDstPts( cv::Point2f( 300, 20 ), cv::Point2f( 300, 420 ), cv::Point2f( 700, 420 ), cv::Point2f( 700, 20 ) );
    cv::Mat perspective_matrix711 = ptrans711.getMatrix();

    perspectiveTransform ptransKymco;
    ptransKymco.setSrcPts( cv::Point2f( 370, 180 ), cv::Point2f( 0, 230 ), cv::Point2f( 650, 415 ), cv::Point2f( 780, 225 ) );
    ptransKymco.setDstPts( cv::Point2f( 300, 20 ), cv::Point2f( 300, 420 ), cv::Point2f( 700, 420 ), cv::Point2f( 700, 20 ) );
    cv::Mat perspective_matrixKymco = ptransKymco.getMatrix();
    // }}}

    /* {{{create originroadMap background */

    cv::Point roadRectTl( 100, 100 ), roadRectBr( 500, 500 );
    cv::rectangle( originRoadMap, roadRectTl, roadRectBr, BLUE_C3, 2 );
    for ( int i = 1; i <= 10; i++ )
    {
        cv::rectangle( originRoadMap, cv::Point( 10, 100 + 35 * i ), cv::Point( 90, 120 + 35 * i ), WHITE_C3, CV_FILLED );
        cv::rectangle( originRoadMap, cv::Point( 510, 100 + 35 * i ), cv::Point( 590, 120 + 35 * i ), WHITE_C3, CV_FILLED );
        cv::rectangle( originRoadMap, cv::Point( 100 + 35 * i, 10 ), cv::Point( 120 + 35 * i, 90 ), WHITE_C3, CV_FILLED );
        cv::rectangle( originRoadMap, cv::Point( 100 + 35 * i, 510 ), cv::Point( 120 + 35 * i, 590 ), WHITE_C3, CV_FILLED );
    }
    putText( originRoadMap , "7-11", cv::Point( 20, 60 ), cv::FONT_HERSHEY_PLAIN, 2,  RED_C3, 2 );
    putText( originRoadMap , "KYMCO", cv::Point( 490, 540 ), cv::FONT_HERSHEY_PLAIN, 2,  RED_C3, 2 );

    /* }}} */

    // {{{creat GMM Class object

    BackgroundSubtractorGMM bsgmm711(  inputImg711.rows, inputImg711.cols );
    bsgmm711.shadowBeBackground = true;

    BackgroundSubtractorGMM bsgmmKymco(  inputImgKymco.rows, inputImgKymco.cols );
    bsgmmKymco.shadowBeBackground = true;

    // }}}

    /* create findRect objects {{{*/
    findRect rect711, rectKymco;
    /* }}} */

    /* create tracker object and points vector {{{*/
    /* float _dt, float _Accel_noise_mag, double _dist_thres = 60, int _maximum_allowed_skipped_frames = 10, int _max_trace_length = 10 */
    CTracker tracker711( 0.2, 0.5, 60, 10, 25 );
    CTracker trackerKymco( 0.2, 0.5, 60, 10, 25 );
    vector<cv::Point2f> trackingPts711Prev, trackingPtsKymcoPrev;
    /* }}} */


    while ( capture711.read( inputImg711 ) && captureKymco.read( inputImgKymco ) )
    {
        /* 711 do GMM operation and do morphologyEx {{{ */
        /* cv::resize( inputImg711, inputImg711, newSize ); */
        bsgmm711.updateFrame( inputImg711.ptr(), outputMask711.ptr() );
        cv::Mat outputMorp711;
        cv::morphologyEx( outputMask711, outputMorp711, CV_MOP_CLOSE, getStructuringElement( cv::MORPH_RECT, cv::Size( 5, 5 ) ) );
        /* }}} */

        /* kymco do GMM operation and do morphologyEx {{{ */
        /* cv::resize( inputImgKymco, inputImgKymco, newSize ); */
        bsgmmKymco.updateFrame( inputImgKymco.ptr(), outputMaskKymco.ptr() );
        cv::Mat outputMorpKymco;
        cv::morphologyEx( outputMaskKymco, outputMorpKymco, CV_MOP_CLOSE, getStructuringElement( cv::MORPH_RECT, cv::Size( 5, 5 ) ) );
        /* }}} */

        /* 711 kymco findBoundingRect, decide points inside rects to be tracked as mapping points on road map {{{ */
        rect711.update( inputImg711, outputMorp711 );
        vector<cv::Rect> boundRect711 = rect711.getRects();
        vector<cv::Point2f> trackingPts711;

        for ( unsigned int i = 0; i < boundRect711.size(); i++ )
        {
            rectangle( inputImg711, boundRect711[i].tl(), boundRect711[i].br(), RED_C3, 2 );
            cv::Point2f mapPts( boundRect711[i].x + boundRect711[i].width / 2, boundRect711[i].y + boundRect711[i].height );
            if ( boundRect711[i].width > boundRect711[i].height )
            {
                mapPts.y -= boundRect711[i].height * 0.15;
            }
            /* cv::circle( inputImg711, mapPts, 4 , GREEN_C3, CV_FILLED ); */
            trackingPts711.push_back( mapPts );
        }
        /* }}} */

        /* 711 update tracker {{{ */
        if ( trackingPts711.size() > 0 )
        {
            tracker711.Update( trackingPts711 );
            trackingPts711Prev = vector<cv::Point2f>( trackingPts711 );
        }
        else
        {
            if ( trackingPts711Prev.size() > 0 )
            {
                tracker711.Update( trackingPts711Prev );
            }
        }
        /* }}} */

        /* 711 show object count, frame number and time stamp {{{ */
        string str711 = "Count:" + to_string( boundRect711.size() ) + " Frame:" + to_string( ( int )capture711.get( CV_CAP_PROP_POS_FRAMES ) ) + "time:" + to_string( ( int )( capture711.get( CV_CAP_PROP_POS_FRAMES ) / FPS ) );
        putText( inputImg711, str711, cv::Point( 300, inputImg711.rows - 20 ), cv::FONT_HERSHEY_PLAIN, 2,  RED_C3, 2 );
        /* }}} */

        /* kymco findBoundingRect, decide points inside rects to be tracked as mapping points on road map {{{ */
        rectKymco.update( inputImgKymco, outputMorpKymco );
        vector<cv::Rect> boundRectKymco =  rectKymco.getRects();
        vector<cv::Point2f> trackingPtsKymco;

        for ( unsigned int i = 0; i < boundRectKymco.size(); i++ )
        {
            rectangle( inputImgKymco, boundRectKymco[i].tl(), boundRectKymco[i].br(), BLUE_C3, 2 );
            cv::Point2f mapPts( boundRectKymco[i].x + boundRectKymco[i].width / 2, boundRectKymco[i].y + boundRectKymco[i].height );
            if ( boundRectKymco[i].width > boundRectKymco[i].height )
            {
                mapPts.y -= boundRectKymco[i].height * 0.1;
            }
            /* cv::circle( inputImgKymco, mapPts, 4 , GREEN_C3, CV_FILLED ); */
            trackingPtsKymco.push_back( mapPts );
        }
        /* }}} */

        /* 711 update tracker {{{ */
        if ( trackingPtsKymco.size() > 0 )
        {
            trackerKymco.Update( trackingPtsKymco );
            trackingPtsKymcoPrev = vector<cv::Point2f>( trackingPtsKymco );
        }
        else
        {
            if ( trackingPtsKymcoPrev.size() > 0 )
            {
                trackerKymco.Update( trackingPtsKymcoPrev );
            }
        }
        /* }}} */

        /* kymco show object count, frame number and time stamp {{{ */
        string strKymco = "Count:" + to_string( boundRectKymco.size() ) + " Frame:" + to_string( ( int )captureKymco.get( CV_CAP_PROP_POS_FRAMES ) ) + "time:" + to_string( ( int )( capture711.get( CV_CAP_PROP_POS_FRAMES ) / FPS ) );
        putText( inputImgKymco, strKymco, cv::Point( 300, inputImg711.rows - 20 ), cv::FONT_HERSHEY_PLAIN, 2,  RED_C3, 2 );
        /* }}} */

        cv::Mat roadMap = originRoadMap.clone();

        /* 711 map tracked points to roadMap{{{ */
        for ( uint i = 0; i < tracker711.tracks.size(); i++ )
        {
            if ( tracker711.tracks[i]->trace.size() > 0 )
            {
                vector<cv::Point2f> dst;
                cv::perspectiveTransform( tracker711.tracks[i]->trace , dst, perspective_matrix711 );
                for ( unsigned int i = 0; i < dst.size(); i++ )
                {
                    cv::Point mappedPt = dst[i] - ptrans711.getDstTl();
                    if ( mappedPt.x >= 0 && mappedPt.x <= roadMap.cols && mappedPt.y >= 0 && mappedPt.y <= roadMap.rows )
                    {
                        mappedPt += roadRectTl;
                        if ( mappedPt.x + mappedPt.y >= 500 || rectKymco.isBurstOrRecovery() )
                        {
                            cv::circle( roadMap, mappedPt, 3 , RED_C3, CV_FILLED );
                        }
                    }
                }
            }
        }
        /* }}} */

        /* kymco map tracked points to roadMap{{{ */
        for ( uint i = 0; i < trackerKymco.tracks.size(); i++ )
        {
            if ( trackerKymco.tracks[i]->trace.size() > 0 )
            {
                vector<cv::Point2f> dst;
                cv::perspectiveTransform( trackerKymco.tracks[i]->trace, dst, perspective_matrixKymco );
                for ( unsigned int i = 0; i < dst.size(); i++ )
                {
                    cv::Point mappedPt = dst[i] - ptransKymco.getDstTl();
                    if ( mappedPt.x >= 0 && mappedPt.x <= roadMap.cols && mappedPt.y >= 0 && mappedPt.y <= roadMap.rows )
                    {
                        /* move first, then rotated 180 degree for both directions */
                        mappedPt += roadRectTl;
                        mappedPt.x = abs( originRoadMap.cols - mappedPt.x );
                        mappedPt.y = abs( originRoadMap.rows - mappedPt.y );
                        if ( mappedPt.x + mappedPt.y <= 600 || rect711.isBurstOrRecovery() )
                        {
                            /* cv::circle( roadMap, mappedPt , 10 , BLUE_C3, CV_FILLED ); */
                            cv::circle( roadMap, mappedPt , 3 , RED_C3, CV_FILLED );
                        }
                    }
                }
            }
        }
        /* }}} */

        /* codes for imshow {{{*/
        if ( Imshow )
        {
            /* cv::imshow( "video711", inputImg711 ); */
            /* cv::imshow( "GMM711", outputMask711 ); */
            /* cv::imshow( "outputMorp711", outputMorp711 ); */
            /* cv::imshow( "videoKymco", inputImgKymco ); */
            /* cv::imshow( "GMMKymco", outputMaskKymco ); */
            /* cv::imshow( "outputMorpKymco", outputMorpKymco ); */
            /* cv::imshow( "roadMap", roadMap ); */
            cv::imshow( "merge", merge );
        }
        /* }}} */

        /* merge windows together {{{ */
        merge.setTo( 0 );
        inputImg711.copyTo( merge( cv::Range( 0, newSize.height ) , cv::Range( 0, newSize.width ) ) );
        inputImgKymco.copyTo( merge( cv::Range( newSize.height + 5, newSize.height * 2 + 5 ) , cv::Range( 0, newSize.width ) ) );
        roadMap.copyTo( merge( cv::Range( 0, roadMap.rows ) , cv::Range( newSize.width + 5, roadMap.cols + newSize.width + 5 ) ) );
        /* }}} */

        if ( outputAvi )
        {
            aw << merge;
        }

        // monitor keys to stop{{{
        if ( cv::waitKey( 1 ) > 0 )
        {
            break;
        }
        //}}}
    }
    bsgmm711.freeMem();
    return EXIT_SUCCESS;
}
