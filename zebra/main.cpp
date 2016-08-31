#include "header.hpp"
#include "standard.hpp"

// {{{sortByX
bool sortByX( cv::RotatedRect i, cv::RotatedRect j )
{
    return i.center.x < j.center.x;
}
// }}}

/// {{{findTanget
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
    for ( unsigned int i = 0; i < pts.size(); i++ )
    {
        numerator += ( ( pts[i].x - x_aver ) * ( pts[i].y - y_aver ) );
        denominator += pow( pts[i].x - x_aver, 2 );
    }
    return numerator / denominator;
}
// }}}

//{{{findZebraAngle
void findZebraAngle( cv::Mat &src, cv::Mat thresholdImg )
{
    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;
    findContours( thresholdImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );
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

        StandardDeviation<cv::RotatedRect> sd( rectCenterPts );
        boundingRectAll = sd.normalize( boundingRectAll, 2, 2 );
        StandardDeviation<cv::Point> sd2( rectCenterPts );
        rectCenterPts = sd2.normalize( rectCenterPts, 2, 2 );

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
        double blueTanget = findTanget( rectCenterPts );
        double blueAngle = abs( atan( -blueTanget ) ) * ( 180 / CV_PI );
        cout << "blue tanget:" << blueTanget << endl;
        cout << "blue angle:" << blueAngle << endl;
        StandardDeviation<cv::Point> getInfo( rectCenterPts );
        double b = getInfo.getyAvg() - getInfo.getxAvg() * blueTanget;
        cv::Point left( 0, b ), bottom( ( src.rows - b ) / blueTanget, src.rows );
        cv::line( src, left, bottom, GREEN_C3, 2 );
        cv::circle( src, cv::Point( getInfo.getxAvg(), getInfo.getyAvg() ), 3, GREEN_C3, 3 );
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

        StandardDeviation<vector<cv::Point>> sd( boundingContoursTopPts );
        boundingContours = sd.normalize( boundingContours, 2, 2 );
        StandardDeviation<cv::Point> sd2( boundingContoursTopPts );
        boundingContoursTopPts = sd2.normalize( boundingContoursTopPts, 2, 2 );

        for ( unsigned int i = 0; i < boundingContoursTopPts.size(); i++ )
        {
            cv::circle( src, boundingContoursTopPts[i], 1, RED_C3, 2 );
            string info = to_string( ( int ) boundingContoursTopPts[i].x ) + "," + to_string( ( int ) boundingContoursTopPts[i].y );
            putText( src, info, cv::Point( boundingContoursTopPts[i].x - 20, boundingContoursTopPts[i].y - 10 ), cv::FONT_HERSHEY_PLAIN, 1,  RED_C3, 2 );
            /* cv::drawContours( src, boundingContours, i, GREEN_C3, 2 ); */
        }
        double redTanget = findTanget( boundingContoursTopPts );
        double redAngle = abs( atan( -redTanget ) ) * ( 180 / CV_PI );
        cout << "red tanget:" << redTanget << endl;
        cout << "red angle:" << redAngle << endl;
        StandardDeviation<cv::Point> getInfo( boundingContoursTopPts );
        double b = getInfo.getyAvg() - getInfo.getxAvg() * redTanget;
        cv::Point left( 0, b ), bottom( ( src.rows - b ) / redTanget, src.rows );
        cv::line( src, left, bottom, GREEN_C3, 2 );
        cv::circle( src, cv::Point( getInfo.getxAvg(), getInfo.getyAvg() ), 3, GREEN_C3, 3 );
    }
}
// }}}

// {{{checkBlack
bool checkBlack( cv::Mat thresholdImg, cv::Point checkPt, int xChkRange, int yChkRange, int threshold )
{
    int cnt = 0;
    /*  x[25] = */
    /* { */
    /*     2, 2, 2, 2, 2, */
    /*     1, 1, 1, 1, 1, */
    /*     0, 0, 0, 0, 0, */
    /*     -1, -1, -1, -1, -1, */
    /*     -2, -2, -2, -2, -2 */
    /* }; */
    /*  y[25] = */
    /* { */
    /*     2, 1, 0, -1, -2, */
    /*     2, 1, 0, -1, -2, */
    /*     2, 1, 0, -1, -2, */
    /*     2, 1, 0, -1, -2, */
    /*     2, 1, 0, -1, -2 */
    /* }; */
    /* for (  i = 0; i < 25; i++ ) */
    int x[9] = {1, 1, 1, 0, 0, 0, -1, -1, -1};
    int y[9] = {1, 0, -1, 1, 0, -1, 1, 0, -1};
    for ( int i = 0; i < 9; i++ )
    {
        if ( thresholdImg.at<uchar>( cv::Point( checkPt.x + x[i] * xChkRange , checkPt.y + y[i] * yChkRange ) ) == ( uchar )0 )
        {
            cnt++;
        }
    }
    return cnt >= threshold ? true : false;
}
// }}}

// {{{ travelPts
vector<cv::Point> travelPts( cv::Mat &src, cv::Mat threshold, int direcX, int direcY )
{
    cv::Point startpt( ( int )( src.cols * 0.5 ), ( int )( src.rows * 0.5 ) );
    cv::circle( src, startpt, 3, GREEN_C3, 2 );
    int xgap = 12 * direcX, ygap = 6 * direcY;
    vector<cv::Point> buffer;
    vector<cv::Point> collect;
    int xRange = 0;
    if ( direcX > 0 )
    {
        xRange = ( int )( ( src.cols - startpt.x ) / abs( xgap ) );
    }
    else
    {
        xRange = ( int )( startpt.x / abs( xgap ) );
    }

    int prevCnt = -1;
    for ( int i = 1; i <= 22; i++ )
    {
        buffer.clear();
        for ( int j = 1; j <= xRange; j++ )
        {
            cv::Point pts( startpt.x + j * xgap, startpt.y + i * ygap );
            if ( checkBlack( threshold, pts , 12 , 6, 8 ) )
            {
                /* cv::circle( src, pts, 1, RED_C3, 2 ); */
                buffer.push_back( pts );
            }
            else
            {
                break;
            }
        }
        if ( prevCnt >= 0 && buffer.size() / 2 >= ( unsigned int )prevCnt )
        {
            break;
        }
        int differ = ( int )( buffer.size() - prevCnt );
        if ( prevCnt >= 0 && differ >= 5 )
        {
            for ( int i = 0; i < differ; i++ )
            {
                buffer.pop_back();
            }
        }
        prevCnt = buffer.size();
        collect.insert( collect.end(), buffer.begin(), buffer.end() );
    }
    return collect;
}
// }}}

// {{{findRoadPts
void findRoadPts( cv::Mat &src, cv::Mat threshold )
{
    vector<cv::Point> collect;
    vector<cv::Point> tlPts = travelPts( src, threshold, -1, -1 );
    vector<cv::Point> trPts = travelPts( src, threshold, -1, 1 );
    vector<cv::Point> blPts = travelPts( src, threshold, 1, -1 );
    vector<cv::Point> brPts = travelPts( src, threshold, 1, 1 );
    collect.insert( collect.end(), tlPts.begin(), tlPts.end() );
    collect.insert( collect.end(), trPts.begin(), trPts.end() );
    collect.insert( collect.end(), blPts.begin(), blPts.end() );
    collect.insert( collect.end(), brPts.begin(), brPts.end() );
    for ( unsigned int i = 0; i < collect.size(); i++ )
    {
        cv::circle( src, collect[i], 1, GREEN_C3, 2 );
    }
    vector<cv::Point> hull;
    cv::convexHull( collect, hull, false );
    for ( unsigned int i = 0; i < hull.size(); i++ )
    {
        cv::circle( src, hull[i], 3, RED_C3, 2 );
        string info = to_string( ( int ) hull[i].x ) + "," + to_string( ( int ) hull[i].y );
        putText( src, info, cv::Point( hull[i].x - 20, hull[i].y - 10 ), cv::FONT_HERSHEY_PLAIN, 1,  RED_C3, 2 );
        cv::line( src, hull[i], hull[( i + 1 ) % hull.size()], BLUE_C3 , 2 );
    }
}
// }}}


int main( int argc, char *argv[] )
{
    cv::Mat input;
    for ( int i = 1; i < argc; i++ )
    {
        cout << "processing:" << argv[i] << endl;
        input = cv::imread( argv[i] );
        cv::resize( input, input, cv::Size( 800, 450 ) );
        cv::Mat src_gray;
        cvtColor( input, src_gray, CV_BGR2GRAY );
        /* imshow( "gray:" + name, src_gray ); */
        cv::Mat thres;
        cv::threshold( src_gray, thres, 180, 255, cv::THRESH_BINARY );
        imshow( "thres:" + string( argv[i] ), thres );
        /* imwrite("thres-"+string( argv[i] ),thres); */
        /* findZebraAngle( input, thres ); */
        findRoadPts( input, thres );
        imshow( "output-" + string( argv[i] ), input );
        imwrite( "output-" + string( argv[i] ), input );
    }
    cv::waitKey( 0 );

    return 0;
}
