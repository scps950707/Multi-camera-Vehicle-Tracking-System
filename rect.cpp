#include "rect.hpp"

/* {{{ findRect::findRect()*/
findRect::findRect()
{
    this->burstLight = false;
    this->recovery = false;
    this->frameRecoveryCnt = 0;
}
/* }}} */

/* removeShadowRect {{{ */
cv::Rect findRect::removeShadowRect ( cv::Rect rect )
{
    int w = 0, h = 0;
    if ( rect.width > rect.height * 2 )
    {
        w = 16;
        h = 8;
    }
    else if ( rect.width * 2 < rect.height )
    {
        w = 8;
        h = 16;
    }
    else
    {
        w = 8;
        h = 8;
    }
    int rw = rect.width;
    int rh = rect.height;
    int tlx = rect.x;
    int tly = rect.y;
    int xbase = rect.tl().x + rect.width / ( 2 * w );
    int ybase = rect.tl().y + rect.height / ( 2 * h );
    for ( int i = 0, leftSideCutCnt = 0; i < w ; i++ )
    {
        int numOfBGSquare = 0;
        for ( int j = 0; j < h; j++ )
        {
            if ( ( uint )mask.at<uchar>( ybase + j * ( rect.height / h ), xbase + i * ( rect.width / w ) ) == 255 )
            {
                /* cv::circle( inputImg, cv::Point( xbase + i * ( rect.width / w ), ybase + j * ( rect.height / h ) ), 1, BLACK_C3, 2 ); */
            }
            else
            {
                /* cv::circle( inputImg, cv::Point( xbase + i * ( rect.width / w ), ybase + j * ( rect.height / h ) ), 1, WHITE_C3, 2 ); */
                numOfBGSquare++;
            }
        }
        if ( numOfBGSquare >= ( int )( 0.75 * h ) )
        {
            if ( leftSideCutCnt == i )
            {
                tlx += rect.width / w;
                leftSideCutCnt++;
            }
            rw -= rect.width / w;
        }
    }
    return cv::Rect( tlx, tly, rw, rh );
}
/* }}} */

/* findRect::update( cv::Mat &inputImg, cv::Mat &mask ) {{{ */
void findRect::update( cv::Mat &inputImg, cv::Mat &mask )
{
    this->inputImg = inputImg;
    this->mask = mask;
    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;
    cv::Mat tmp;
    mask.copyTo( tmp );
    findContours( tmp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );
    vector<vector<cv::Point>> contours_poly( contours.size() );
    int whitePixelCnt = 0;
    for ( int i = 0; i < mask.rows; i++ )
    {
        for ( int j = 0; j < mask.cols; j++ )
        {
            if ( mask.at<uchar>( i, j ) == ( uchar )255 )
            {
                whitePixelCnt++;
            }
        }
    }
    int percentage = ( int )( ( ( double )whitePixelCnt / ( double )( mask.cols * mask.rows ) ) * 100 );
    if ( percentage >= 30 )
    {
        putText( inputImg, "Warning:burst light", cv::Point( 30, 50 ), CV_FONT_HERSHEY_SIMPLEX, 2, RED_C3, 3 );
        this->burstLight = true;
        this->rects.clear();
        this->rectCenters.clear();
        this->rectCentersFloat.clear();
        return;
    }
    else
    {
        if ( this->burstLight )
        {
            putText( inputImg, "start burst light recovery", cv::Point( 30, 50 ), CV_FONT_HERSHEY_SIMPLEX, 2, RED_C3, 3 );
            this->burstLight = false;
            this->recovery = true;
            frameRecoveryCnt = 40;
            return;
        }
        else if ( this->recovery )
        {
            putText( inputImg, "burst light recovery", cv::Point( 30, 50 ), CV_FONT_HERSHEY_SIMPLEX, 2, RED_C3, 3 );
            if ( --frameRecoveryCnt == 0 )
            {
                this->recovery = false;
                this->burstLight = false;
            }
            return;
        }
    }

    for ( unsigned int i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( cv::Mat( contours[i] ), contours_poly[i], 3, true );
    }

    auto removethese = std::remove_if( contours_poly.begin(), contours_poly.end(), []( vector<cv::Point> contour )
    {
        return cv::contourArea( contour ) < 300;
    } );
    contours_poly.erase( removethese, contours_poly.end() );

    rects.resize( contours_poly.size() );
    std::transform( contours_poly.begin(), contours_poly.end(), rects.begin(),
                    []( vector<cv::Point> contour )
    {
        return cv::boundingRect( cv::Mat( contour ) );
    }
                  );

    rectCenters.resize( contours_poly.size() );
    std::transform( rects.begin(), rects.end(), rectCenters.begin(),
                    []( cv::Rect rect )
    {
        return cv::Point( rect.x + rect.width * 0.5, rect.y + rect.height * 0.5 );
    }
                  );

    rectCentersFloat.resize( contours_poly.size() );
    std::transform( rects.begin(), rects.end(), rectCentersFloat.begin(),
                    []( cv::Rect rect )
    {
        return cv::Point2f( rect.x + rect.width * 0.5, rect.y + rect.height * 0.5 );
    }
                  );

}
/* }}} */

/* vector<cv::Point> findRect::getRectCenters() {{{*/
vector<cv::Point> findRect::getRectCenters()
{
    return this->rectCenters;
}
/* }}} */

/* vector<cv::Point> findRect::getRectCentersFloat() {{{*/
vector<cv::Point2f> findRect::getRectCentersFloat()
{
    return this->rectCentersFloat;
}

/* }}} */

/* vector<cv::Rect> findRect::getRects() {{{*/
vector<cv::Rect> findRect::getRects()
{
    return this->rects;
}
/* }}} */

/* findRect::isBurstOrRecovery() {{{*/
bool findRect::isBurstOrRecovery()
{
    return ( this->recovery || this->burstLight );
}
/* }}} */
