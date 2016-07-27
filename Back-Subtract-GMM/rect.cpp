#include "rect.hpp"
#include<stdio.h>
#include<cmath>

void findRect::findBoundingRect( cv::Mat &rectTarget, cv::Mat &mask )
{
    cv::Mat tmp;
    mask.copyTo( tmp );
    findContours( tmp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
    vector<vector<cv::Point>> contours_poly( contours.size() );
    vector<cv::Rect> boundRect;
    for ( unsigned int i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( cv::Mat( contours[i] ), contours_poly[i], 3, true );
        cv::Rect newRect = boundingRect( cv::Mat( contours_poly[i] ) );
        if ( newRect.area() < 1500 )
        {
            continue;
        }
        boundRect.push_back( newRect );
    }
    char str[20];
    sprintf( str, "Count:%lu", boundRect.size() );
    putText( rectTarget, str, cv::Point( 300, rectTarget.rows - 20 ), cv::FONT_HERSHEY_PLAIN, 2,  cv::Scalar( 0, 0, 255 ), 2 );
    for ( unsigned int i = 0; i < boundRect.size(); i++ )
    {
        if ( ( int )( boundRect[i].width / boundRect[i].height ) >= 4 || ( int )( boundRect[i].height / boundRect[i].width ) >= 4 )
        {
            continue;
        }
        char box[20];
        sprintf( box, "%dx%d=%d", boundRect[i].width, boundRect[i].height, boundRect[i].area() );
        putText( rectTarget, box, boundRect[i].br(), cv::FONT_HERSHEY_PLAIN, 1,  cv::Scalar( 0, 0, 255 ), 2 );
        rectangle( rectTarget, boundRect[i].tl(), boundRect[i].br(), cv::Scalar( 0, 0, 255 ), 2, 8, 0 );
    }
}
