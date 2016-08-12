#include "header.hpp"

bool sortByX( cv::RotatedRect i, cv::RotatedRect j )
{
    return i.center.x < j.center.x;
}

double findTanget( vector<cv::Point> pts )
{
    double x_aver = 0, y_aver = 0;
    for ( unsigned int i = 0; i < pts.size(); i++ )
    {
        x_aver += pts[i].x;
        y_aver += pts[i].y;
    }
    x_aver /= pts.size();
    y_aver /= pts.size();
    double numerator = 0, denominator = 0;
    /* double tmpDenX = 0, tmpDenY = 0; */
    for ( unsigned int i = 0; i < pts.size(); i++ )
    {
        numerator += ( ( pts[i].x - x_aver ) * ( pts[i].y - y_aver ) );
        denominator += pow( pts[i].x - x_aver, 2 );
        /* tmpDenX += pow( pts[i.x - x_aver, 2 ); */
        /* tmpDenY += pow( pts[i.y - y_aver, 2 ); */
    }
    /* denominator = sqrt( tmpDenX ) * sqrt( tmpDenY ); */
    return numerator / denominator;
}

template <typename X>
void normalizeUsingStandardDeviation( vector<X> &target , vector<cv::Point> pts, int xRange, int yRange )
{
    double xAvg = 0;
    double xSigma = 0;
    double yAver = 0;
    double ySigma = 0;
    for ( unsigned int i = 0; i < pts.size(); i++ )
    {
        xAvg += pts[i].x;
        yAver += pts[i].y;
    }
    xAvg /= pts.size();
    yAver /= pts.size();
    for ( unsigned int i = 0; i < pts.size(); i++ )
    {
        xSigma += pow( pts[i].x - xAvg, 2 );
        ySigma += pow( pts[i].y - yAver, 2 );
    }
    xSigma /= pts.size();
    xSigma = sqrt( xSigma );
    ySigma /= pts.size();
    ySigma = sqrt( ySigma );
    /* cout << "xAvg:" << xAvg << "xSigma:" << xSigma << endl; */
    /* cout << "yAver:" << yAver << "ySigma:" << ySigma << endl; */
    for ( unsigned int i = 0; i < pts.size(); i++ )
    {
        if ( pts[i].x < ( xAvg - ( xRange * xSigma ) ) || pts[i].x > ( xAvg + ( xRange * xSigma ) )
                || pts[i].y < ( yAver - ( yRange * ySigma ) ) || pts[i].y > ( yAver + ( yRange * ySigma ) ) )
        {
            target.erase( target.begin() + i );
        }
    }
}

void findZebra( cv::Mat &src, string name )
{
    cv::Mat src_gray;

    cvtColor( src, src_gray, CV_BGR2GRAY );
    /* imshow( "gray:" + name, src_gray ); */
    cv::Mat thres;
    cv::threshold( src_gray, thres, 140, 255, cv::THRESH_BINARY );
    for ( int i = 0; i < ( int )( thres.rows * 0.4 ); i++ )
    {
        for ( int j = 0; j < thres.cols; j++ )
        {
            thres.at<uchar>( i, j ) = ( uchar )0;
        }
    }
    /* imshow( "thres:" + name, thres ); */

    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;
    findContours( thres, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );
    vector<vector<cv::Point>> contours_poly( contours.size() );
    vector<cv::RotatedRect> boundingRectAll;
    vector<vector<cv::Point>> boundingContours;
    for ( unsigned int i = 0; i < contours.size(); i++ )
    {
        approxPolyDP( cv::Mat( contours[i] ), contours_poly[i], 3, true );
        if ( cv::contourArea( contours_poly[i] ) > 500 && cv::contourArea( contours_poly[i] ) < 10000 && ( int )contours_poly[i].size() < 50 )
        {
            cv::RotatedRect rect = cv::minAreaRect( cv::Mat( contours_poly[i] ) );
            if ( rect.center.y > 0.4 * src.rows && rect.center.x < 0.75 * src.cols &&  rect.center.x > 0.05 * src.cols )
            {
                boundingRectAll.push_back( rect );
                boundingContours.push_back( contours_poly[i] );
            }
            /* cout << "x,y:" << ( int )rect.center.x << "," << ( int )rect.center.y << "con area:" << cv::contourArea( contours_poly[i] ) << "rect:" << ( int )rect.size.area() << endl; */
        }
    }

    if ( !boundingRectAll.empty() )
    {
        vector<cv::Point> rectCenterPts;
        for ( unsigned int i = 0; i < boundingRectAll.size(); i++ )
        {
            rectCenterPts.push_back( boundingRectAll[i].center );
        }

        normalizeUsingStandardDeviation( boundingRectAll, rectCenterPts, 2, 2 );

        for ( unsigned int i = 0; i < boundingRectAll.size(); i++ )
        {
            cv::circle( src, boundingRectAll[i].center, 1, BLUE_C3, 2 );
            string info = to_string( ( int ) boundingRectAll[i].center.x ) + "," + to_string( ( int ) boundingRectAll[i].center.y );
            putText( src, info, cv::Point( boundingRectAll[i].center.x - 20, boundingRectAll[i].center.y - 10 ), cv::FONT_HERSHEY_PLAIN, 1,  BLUE_C3, 2 );
            /* cv::Point2f vertice[4]; */
            /* boundingRectAll[i].points( vertice ); */
            /* for ( int j = 0; j < 4; j++ ) */
            /* { */
            /*     cv::line( src, vertice[j], vertice[( j + 1 ) % 4], BLUE_C3 , 2 ); */
            /* } */
        }
        cout << "blue angle:" << atan( -( findTanget( rectCenterPts ) ) ) * ( 180 / CV_PI ) << endl;
    }
    if ( !boundingContours.empty() )
    {
        vector<cv::Point> boundingContoursTopPts;
        for ( unsigned int i = 0; i < boundingContours.size(); i++ )
        {
            int maxIdx = 0;
            for ( unsigned int j = 0; j < boundingContours[i].size(); j++ )
            {
                if ( boundingContours[i][j].y < boundingContours[i][maxIdx].y )
                {
                    maxIdx = j;
                }
            }
            boundingContoursTopPts.push_back( boundingContours[i][maxIdx] );
        }

        normalizeUsingStandardDeviation( boundingContours, boundingContoursTopPts, 2, 2 );
        normalizeUsingStandardDeviation( boundingContoursTopPts, boundingContoursTopPts, 2, 2 );

        for ( unsigned int i = 0; i < boundingContoursTopPts.size(); i++ )
        {
            cv::circle( src, boundingContoursTopPts[i], 1, RED_C3, 2 );
            string info = to_string( ( int ) boundingContoursTopPts[i].x ) + "," + to_string( ( int ) boundingContoursTopPts[i].y );
            putText( src, info, cv::Point( boundingContoursTopPts[i].x - 20, boundingContoursTopPts[i].y - 10 ), cv::FONT_HERSHEY_PLAIN, 1,  RED_C3, 2 );
            /* cv::drawContours( src, boundingContours, i, GREEN_C3, 2 ); */
        }
        cout << "red angle:" << atan( -( findTanget( boundingContoursTopPts ) ) ) * ( 180 / CV_PI ) << endl;
    }
    imshow( "origin:" + name, src );
}

int main( int argc, char *argv[] )
{
    cv::Mat input;
    for ( int i = 1; i < argc; i++ )
    {
        cout << "processing:" << argv[i] << endl;
        input = cv::imread( argv[i] );
        findZebra( input, string( argv[i] ) );
    }
    cv::waitKey( 0 );

    return 0;
}
