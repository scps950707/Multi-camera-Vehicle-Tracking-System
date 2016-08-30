#include "header.hpp"

int main( int argc, char *argv[] )
{
    // codes for control command line options {{{
    int  options;
    bool isSeven = false, isKymco = false;
    string imPath;
    string fileNamePrefix;
    struct option  long_opt[] =
    {
        {"711", required_argument, NULL, 's'},
        {"kymco", required_argument, NULL, 'k'},
        {NULL, 0, NULL, 0}
    };
    while ( ( options = getopt_long( argc, argv, "s:k:", long_opt, NULL ) ) != -1 )
    {
        switch  ( options )
        {
        case 's':
            isSeven = true;
            imPath = string( optarg );
            fileNamePrefix = "7-11";
            break;
        case 'k':
            isKymco = true;
            imPath = string( optarg );
            fileNamePrefix = "kymco";
            break;
        }
    }
    // }}}

    cv::Point2f srcPts[4], dstPts[4];
    cv::Mat src_img = cv::imread( imPath, 1 );
    if ( src_img.empty() )
    {
        return EXIT_FAILURE;
    }
    cv::resize( src_img, src_img, cv::Size( 800, 450 ) );
    /* set coordinate for source points to target points */
    /* order:top left,bottom left,bottom right,top right */
    /* kmyco.jpg */
    if ( isKymco )
    {
        srcPts[0] = cv::Point2f( 370, 190 );
        srcPts[1] = cv::Point2f( 0, 230 );
        srcPts[2] = cv::Point2f( 650, 410 );
        srcPts[3] = cv::Point2f( 780, 225 );

        dstPts[0] = cv::Point2f( 300, 20 );
        dstPts[1] = cv::Point2f( 300, 420 );
        dstPts[2] = cv::Point2f( 700, 420 );
        dstPts[3] = cv::Point2f( 700, 20 );
    }
    /* 7-11.jpg */
    else if ( isSeven )
    {
        srcPts[0] = cv::Point2f( 330, 100 );
        srcPts[1] = cv::Point2f( 0, 180 );
        srcPts[2] = cv::Point2f( 730, 390 );
        srcPts[3] = cv::Point2f( 660, 145 );

        dstPts[0] = cv::Point2f( 300, 20 );
        dstPts[1] = cv::Point2f( 300, 420 );
        dstPts[2] = cv::Point2f( 700, 420 );
        dstPts[3] = cv::Point2f( 700, 20 );
    }

    // get perspectiveTransform matrix
    cv::Mat perspective_matrix = cv::getPerspectiveTransform( srcPts, dstPts );
    cv::Mat dst_img;
    // do perspectiveTransform on image
    cv::warpPerspective( src_img, dst_img, perspective_matrix, src_img.size(), cv::INTER_LINEAR );
    // draw range lines
    cv::line( src_img, srcPts[0], srcPts[1], cv::Scalar( 255, 255, 0 ), 2, CV_AA );
    cv::line( src_img, srcPts[1], srcPts[2], cv::Scalar( 255, 255, 0 ), 2, CV_AA );
    cv::line( src_img, srcPts[2], srcPts[3], cv::Scalar( 255, 255, 0 ), 2, CV_AA );
    cv::line( src_img, srcPts[3], srcPts[0], cv::Scalar( 255, 255, 0 ), 2, CV_AA );
    cv::line( src_img, dstPts[0], dstPts[1], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( src_img, dstPts[1], dstPts[2], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( src_img, dstPts[2], dstPts[3], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( src_img, dstPts[3], dstPts[0], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( dst_img, dstPts[0], dstPts[1], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( dst_img, dstPts[1], dstPts[2], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( dst_img, dstPts[2], dstPts[3], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( dst_img, dstPts[3], dstPts[0], cv::Scalar( 255, 0, 255 ), 2, CV_AA );

    vector<cv::Point2f> ori;
    for ( int i = 50; i <= src_img.cols; i += ( src_img.cols - 50 ) / 20 )
    {
        for ( int j = 100; j <= src_img.rows; j += ( src_img.cols - 50 ) / 30 )
        {
            ori.push_back( cv::Point2f( i, j ) );
        }
    }
    vector<cv::Point2f> dst;
    cv::perspectiveTransform( ori, dst, perspective_matrix );
    cv::RNG rng( 12345 );
    for ( unsigned int i = 0; i < dst.size(); i++ )
    {
        cv::Scalar randColor( rng.uniform( 0, 255 ), rng.uniform( 0, 255 ), rng.uniform( 0, 255 ) );
        cv::circle( src_img, ori[i], 3, randColor, CV_FILLED );
        cv::circle( dst_img, dst[i], 3, randColor, CV_FILLED );
    }

    cv::imshow( "input-perspectiveTransform-" + fileNamePrefix + ".jpg", src_img );
    /* cv::imwrite( "input-perspectiveTransform-"+fileNamePrefix+".jpg", src_img ); */
    cv::imshow( "output-perspectiveTransform-" + fileNamePrefix, dst_img );
    /* cv::imwrite( "output-perspectiveTransform-"+fileNamePrefix+".jpg", dst_img ); */

    cv::waitKey( 0 );
}
