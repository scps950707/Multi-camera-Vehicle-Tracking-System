#include "header.hpp"

int main( int argc, char *argv[] )
{
    cv::Point2f pts1[4], pts2[4];
    cv::Mat src_img = cv::imread( argv[1], 1 );
    if ( src_img.empty() )
    {
        return EXIT_FAILURE;
    }
    if ( src_img.cols > 1800 && src_img.rows > 900 )
    {
        cv::resize( src_img, src_img, cv::Size( src_img.cols / 2, src_img.rows / 2 ) );
    }
    // 設定變換[之前]與[之後]的坐標 (左上,左下,右下,右上)
    //kmyco.jpg
//    pts1[0] = cv::Point2f( 447, 256 );
//    pts1[1] = cv::Point2f( 119, 280 );
//    pts1[2] = cv::Point2f( 750, 560 );
//    pts1[3] = cv::Point2f( 822, 314 );
//
//    pts2[0] = cv::Point2f( 375, 225 );
//    pts2[1] = cv::Point2f( 305, 470 );
//    pts2[2] = cv::Point2f( 810, 465 );
//    pts2[3] = cv::Point2f( 810, 220 );
    //7-11.jpg
    /* pts1[0] = cv::Point2f( 347, 246 ); */
    /* pts1[1] = cv::Point2f( 21, 335 ); */
    /* pts1[2] = cv::Point2f( 854, 430 ); */
    /* pts1[3] = cv::Point2f( 754, 239 ); */

    /* pts2[0] = cv::Point2f( 152, 185 ); */
    /* pts2[1] = cv::Point2f( 80, 465 ); */
    /* pts2[2] = cv::Point2f( 824, 452 ); */
    /* pts2[3] = cv::Point2f( 775, 185 ); */

    //DLSR.jpg
    pts1[0] = cv::Point2f( 420, 167 );
    pts1[1] = cv::Point2f( 0, 208 );
    pts1[2] = cv::Point2f( 741, 457 );
    pts1[3] = cv::Point2f( 920, 230 );

    pts2[0] = cv::Point2f( 340, 100 );
    pts2[1] = cv::Point2f( 340, 520 );
    pts2[2] = cv::Point2f( 760, 520 );
    pts2[3] = cv::Point2f( 760, 100 );
    // 透視變換行列計算
    cv::Mat perspective_matrix = cv::getPerspectiveTransform( pts1, pts2 );
    cv::Mat dst_img;
    // 變換
    cv::warpPerspective( src_img, dst_img, perspective_matrix, src_img.size(), cv::INTER_LINEAR );
    // 繪製坐標變換[之前]與[之後]的示意圖
    cv::line( src_img, pts1[0], pts1[1], cv::Scalar( 255, 255, 0 ), 2, CV_AA );
    cv::line( src_img, pts1[1], pts1[2], cv::Scalar( 255, 255, 0 ), 2, CV_AA );
    cv::line( src_img, pts1[2], pts1[3], cv::Scalar( 255, 255, 0 ), 2, CV_AA );
    cv::line( src_img, pts1[3], pts1[0], cv::Scalar( 255, 255, 0 ), 2, CV_AA );
    cv::line( src_img, pts2[0], pts2[1], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( src_img, pts2[1], pts2[2], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( src_img, pts2[2], pts2[3], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( src_img, pts2[3], pts2[0], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( dst_img, pts2[0], pts2[1], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( dst_img, pts2[1], pts2[2], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( dst_img, pts2[2], pts2[3], cv::Scalar( 255, 0, 255 ), 2, CV_AA );
    cv::line( dst_img, pts2[3], pts2[0], cv::Scalar( 255, 0, 255 ), 2, CV_AA );

    vector<cv::Point2f> ori;
    for ( int i = 50; i <= src_img.cols; i += ( src_img.cols - 50 ) / 20 )
    {
        for ( int j = 100; j <= src_img.rows; j += ( src_img.cols - 50 ) / 20 )
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
        cv::circle( src_img, ori[i], 2, randColor, 2 );
        cv::circle( dst_img, dst[i], 2, randColor, 2 );
    }

    cv::imshow( "src", src_img );
    cv::imshow( "dst", dst_img );

    cv::waitKey( 0 );
}
