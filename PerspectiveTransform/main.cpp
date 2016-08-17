/**
    Theme: Perspective Transform
    compiler: Visual C++ 2010 with OpenCV 2.4.3
    Date: 102/03/22
    Author: HappyMan
    Blog: https://cg2010studio.wordpress.com/
*/
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
const int nOffset = 180; // 設定位移幅度

int main( int argc, char *argv[] )
{
    cv::Point2f pts1[4], pts2[4];
    cv::Mat src_img = cv::imread( argv[1], 1 );
    if ( src_img.empty() )
    {
        return -1;
    }
    // 設定變換[之前]與[之後]的坐標 (左上,左下,右下,右上)
    pts1[0] = {cv::Point2f( 0, 0 )};
    pts1[1] = {cv::Point2f( 0, src_img.rows )};
    pts1[2] = {cv::Point2f( src_img.cols, src_img.rows )};
    pts1[3] = {cv::Point2f( src_img.cols, 0 )};

    pts2[0] = {cv::Point2f( 0, 0 )};
    pts2[1] = {cv::Point2f( 0 + nOffset, src_img.rows )};
    pts2[2] = {cv::Point2f( src_img.cols - nOffset, src_img.rows )};
    pts2[3] = {cv::Point2f( src_img.cols, 0 )};

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

    cv::namedWindow( "src", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO );
    cv::namedWindow( "dst", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO );
    cv::imshow( "src", src_img );
    cv::imshow( "dst", dst_img );
    cv::waitKey( 0 );
}
