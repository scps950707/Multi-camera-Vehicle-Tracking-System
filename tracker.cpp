#include "tracker.hpp"

/* Tracker::CTraerckcartTt cv::Point2f &p, float dt, float accelNoiseMag, int trackID ) {{{*/
Tracker::Tracker( const cv::Point2f &p, float dt, float accelNoiseMag, int trackID )
    :
    trackId( trackID ),
    skippedFrames( 0 ),
    prediction( p ),
    KF( p, dt, accelNoiseMag )
{
}
/* }}} */

/* float Tracker::calcDist( const cv::Point2f &p ) {{{*/
float Tracker::calcDist( const cv::Point2f &p )
{
    cv::Point2f diff = prediction - p;
    return sqrtf( diff.x * diff.x + diff.y * diff.y );
}
/* }}} */

/* void Tracker::update( const cv::Point2f &p, bool dataCorrect, int maxTraceLength ) {{{*/
void Tracker::update( const cv::Point2f &p, bool dataCorrect, int maxTraceLength )
{
    KF.GetPrediction();
    prediction = KF.Update( p, dataCorrect );

    if ( trackedPts.size() > ( uint )maxTraceLength )
    {
        trackedPts.erase( trackedPts.begin(), trackedPts.end() - maxTraceLength );
    }

    trackedPts.push_back( prediction );
}
/* }}} */
