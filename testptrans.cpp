#include "header.hpp"
#include "ptrans.hpp"

int main( int argc, char *argv[] )
{
    // codes for control command line options {{{
    if ( argc < 2 )
    {
        cout << "usage: ./exe -s 711.jpg -k kymco.jpg" << endl;
        exit( EXIT_FAILURE );
    }

    int  options;
    string path711 = "", pathKymco = "";
    cv::RNG rng( 12345 );
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
            path711 = string( optarg );
            break;
        case 'k':
            pathKymco = string( optarg );
            break;
        }
    }
    // }}}

    if ( path711 != "" )
    {

        perspectiveTransform ptrans711;
        ptrans711.setSrcPts( cv::Point2f( 330, 95 ),
                             cv::Point2f( 0, 180 ),
                             cv::Point2f( 745, 410 ),
                             cv::Point2f( 665, 145 ) );

        ptrans711.setDstPts( cv::Point2f( 300, 20 ),
                             cv::Point2f( 300, 420 ),
                             cv::Point2f( 700, 420 ),
                             cv::Point2f( 700, 20 ) );

        cv::Mat matrix711 = ptrans711.getMatrix();
        cv::Mat img711 = cv::imread( path711 );
        cv::resize( img711, img711, cv::Size( 800, 450 ) );
        cv::Mat out711( img711.size(), img711.type() );
        cv::warpPerspective( img711, out711, matrix711, img711.size(), cv::INTER_LINEAR );
        cv::line( img711, ptrans711.getSrcTl(), ptrans711.getSrcBl(), cv::Scalar( 255, 255, 0 ), 2, CV_AA );
        cv::line( img711, ptrans711.getSrcBl(), ptrans711.getSrcBr(), cv::Scalar( 255, 255, 0 ), 2, CV_AA );
        cv::line( img711, ptrans711.getSrcBr(), ptrans711.getSrcTr(), cv::Scalar( 255, 255, 0 ), 2, CV_AA );
        cv::line( img711, ptrans711.getSrcTr(), ptrans711.getSrcTl(), cv::Scalar( 255, 255, 0 ), 2, CV_AA );
        cv::line( img711, ptrans711.getDstTl(), ptrans711.getDstBl(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( img711, ptrans711.getDstBl(), ptrans711.getDstBr(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( img711, ptrans711.getDstBr(), ptrans711.getDstTr(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( img711, ptrans711.getDstTr(), ptrans711.getDstTl(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( out711, ptrans711.getDstTl(), ptrans711.getDstBl(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( out711, ptrans711.getDstBl(), ptrans711.getDstBr(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( out711, ptrans711.getDstBr(), ptrans711.getDstTr(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( out711, ptrans711.getDstTr(), ptrans711.getDstTl(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );

        vector<cv::Point2f> ori;
        for ( int i = 50; i <= img711.cols; i += ( img711.cols - 50 ) / 20 )
        {
            for ( int j = 100; j <= img711.rows; j += ( img711.cols - 50 ) / 30 )
            {
                ori.push_back( cv::Point2f( i, j ) );
            }
        }
        vector<cv::Point2f> dst;
        cv::perspectiveTransform( ori, dst, matrix711 );
        for ( unsigned int i = 0; i < dst.size(); i++ )
        {
            cv::Scalar randColor( rng.uniform( 0, 255 ), rng.uniform( 0, 255 ), rng.uniform( 0, 255 ) );
            cv::circle( img711, ori[i], 3, randColor, CV_FILLED );
            cv::circle( out711, dst[i], 3, randColor, CV_FILLED );
        }

        cv::imshow( "711-ori", img711 );
        /* cv::imwrite( "711-ori.jpg", img711 ); */
        cv::imshow( "711-out", out711 );
        /* cv::imwrite( "711-out.jpg", out711 ); */
    }

    if ( pathKymco != "" )
    {
        perspectiveTransform ptransKymco;
        ptransKymco.setSrcPts( cv::Point2f( 370, 180 ),
                               cv::Point2f( 0, 230 ),
                               cv::Point2f( 650, 415 ),
                               cv::Point2f( 780, 225 ) );

        ptransKymco.setDstPts( cv::Point2f( 300, 20 ),
                               cv::Point2f( 300, 420 ),
                               cv::Point2f( 700, 420 ),
                               cv::Point2f( 700, 20 ) );

        cv::Mat matrixKymco = ptransKymco.getMatrix();
        cv::Mat imgKymco = cv::imread( pathKymco );
        cv::resize( imgKymco, imgKymco, cv::Size( 800, 450 ) );
        cv::Mat outKymco( imgKymco.size(), imgKymco.type() );
        cv::warpPerspective( imgKymco, outKymco, matrixKymco, imgKymco.size(), cv::INTER_LINEAR );
        cv::line( imgKymco, ptransKymco.getSrcTl(), ptransKymco.getSrcBl(), cv::Scalar( 255, 255, 0 ), 2, CV_AA );
        cv::line( imgKymco, ptransKymco.getSrcBl(), ptransKymco.getSrcBr(), cv::Scalar( 255, 255, 0 ), 2, CV_AA );
        cv::line( imgKymco, ptransKymco.getSrcBr(), ptransKymco.getSrcTr(), cv::Scalar( 255, 255, 0 ), 2, CV_AA );
        cv::line( imgKymco, ptransKymco.getSrcTr(), ptransKymco.getSrcTl(), cv::Scalar( 255, 255, 0 ), 2, CV_AA );
        cv::line( imgKymco, ptransKymco.getDstTl(), ptransKymco.getDstBl(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( imgKymco, ptransKymco.getDstBl(), ptransKymco.getDstBr(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( imgKymco, ptransKymco.getDstBr(), ptransKymco.getDstTr(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( imgKymco, ptransKymco.getDstTr(), ptransKymco.getDstTl(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( outKymco, ptransKymco.getDstTl(), ptransKymco.getDstBl(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( outKymco, ptransKymco.getDstBl(), ptransKymco.getDstBr(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( outKymco, ptransKymco.getDstBr(), ptransKymco.getDstTr(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );
        cv::line( outKymco, ptransKymco.getDstTr(), ptransKymco.getDstTl(), cv::Scalar( 255, 0, 255 ), 2, CV_AA );

        vector<cv::Point2f> ori;
        for ( int i = 50; i <= imgKymco.cols; i += ( imgKymco.cols - 50 ) / 20 )
        {
            for ( int j = 100; j <= imgKymco.rows; j += ( imgKymco.cols - 50 ) / 30 )
            {
                ori.push_back( cv::Point2f( i, j ) );
            }
        }
        vector<cv::Point2f> dst;
        cv::perspectiveTransform( ori, dst, matrixKymco );
        for ( unsigned int i = 0; i < dst.size(); i++ )
        {
            cv::Scalar randColor( rng.uniform( 0, 255 ), rng.uniform( 0, 255 ), rng.uniform( 0, 255 ) );
            cv::circle( imgKymco, ori[i], 3, randColor, CV_FILLED );
            cv::circle( outKymco, dst[i], 3, randColor, CV_FILLED );
        }

        cv::imshow( "kymco-ori", imgKymco );
        /* cv::imwrite( "kymco-ori.jpg", imgKymco ); */
        cv::imshow( "kymco-out", outKymco );
        /* cv::imwrite( "kymco-out.jpg", outKymco ); */
    }
    cv::waitKey( 0 );
}
