#include "rect.hpp"
#include<stdio.h>
#include<cmath>

int findRect::containTargetPerCent( cv::Rect container, cv::Rect target )
{
    int interSectionArea = ( container & target ).area();
    if ( interSectionArea == 0 )
    {
        return 0;
    }
    else if ( interSectionArea == target.area() )
    {
        return 100;
    }
    else
    {
        return int( ( interSectionArea / target.area() ) * 100 );
    }
}

void findRect::findBoundingRect( cv::Mat &rectTarget, cv::Mat &mask )
{
    cv::Mat tmp;
    mask.copyTo( tmp );
    findContours( tmp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cv::Point( 0, 0 ) );
    vector<vector<cv::Point>> contours_poly( contours.size() );
    vector<cv::Rect> boundRect;
    for ( unsigned int i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( cv::Mat( contours[i] ), contours_poly[i], 3, true );
        cv::Rect newRect = boundingRect( cv::Mat( contours_poly[i] ) );
        if ( newRect.width < 15 || newRect.height < 50 )
        {
            continue;
        }
        else if ( abs( newRect.width - rectTarget.cols ) < 20 && abs( newRect.height - rectTarget.rows ) < 20 )
        {
            continue;
        }
        bool addRect = true;
        for ( unsigned int j = 0; j < boundRect.size(); j++ )
        {
            if ( containTargetPerCent( boundRect[j], newRect ) >= 60 )
            {
                boundRect[j] = ( boundRect[j] | newRect );
                addRect = false;
            }
            if ( containTargetPerCent( newRect , boundRect[j] ) == 100 )
            {
                boundRect[j] = newRect;
                addRect = false;
            }
        }
        if ( addRect )
        {
            boundRect.push_back( newRect );
        }
    }
    char str[20];
    sprintf( str, "Count:%lu", boundRect.size() );
    putText( rectTarget, str, cv::Point( 300, rectTarget.rows - 20 ), cv::FONT_HERSHEY_PLAIN, 2,  cv::Scalar( 0, 0, 255 ), 2 );
    for ( unsigned int i = 0; i < boundRect.size(); i++ )
    {
        rectangle( rectTarget, boundRect[i].tl(), boundRect[i].br(), cv::Scalar( 0, 0, 255 ), 2, 8, 0 );
    }
}
