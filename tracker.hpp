#ifndef __TRACKER_HPP__
#define __TRACKER_HPP__

#include "header.hpp"
#include "kalman.hpp"

class Tracker
{
public:
    Tracker( const cv::Point2f &p, float dt, float accelNoiseMag, int trackID );
    float calcDist( const cv::Point2f &p );
    void update( const cv::Point2f &p, bool dataCorrect, int maxTraceLength );
    vector<cv::Point2f> trackedPts;
    int trackId;
    int skippedFrames;
private:
    cv::Point2f prediction;
    TKalmanFilter KF;
};

#endif
