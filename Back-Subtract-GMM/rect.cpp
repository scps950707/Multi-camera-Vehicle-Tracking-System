#include "rect.hpp"


void findRect::findBoundingRect( cv::Mat &rectTarget, cv::Mat &mask )
{
    cv::Mat tmp;
    mask.copyTo( tmp );
    findContours( tmp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cv::Point( 0, 0 ) );
    vector<vector<cv::Point>> contours_poly( contours.size() );
    vector<cv::Rect> boundRect( contours.size() );
    for ( unsigned int i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( cv::Mat( contours[i] ), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( cv::Mat( contours_poly[i] ) );
    }
    for ( unsigned int i = 0; i < contours.size(); i++ )
    {
        if ( boundRect[i].area() < 2500 )
        {
            continue;
        }
        else if ( abs( boundRect[i].width - rectTarget.cols ) < 20 && abs( boundRect[i].height - rectTarget.rows ) < 20 )
        {
            continue;
        }
        rectangle( rectTarget, boundRect[i].tl(), boundRect[i].br(), cv::Scalar( 0, 0, 255 ), 2, 8, 0 );
    }
}
