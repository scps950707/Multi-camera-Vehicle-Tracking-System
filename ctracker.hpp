#ifndef __CTRACKER_HPP__
#define __CTRACKER_HPP__

#include "kalman.hpp"
#include "hungarianAlg.hpp"
#include "header.hpp"
#include <memory>

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

class MultiTracker
{
public:
    MultiTracker( float dt, float accelNoiseMag, float distThres = 60, int maxiumAllowedSkippedFrames = 10, int maxTraceLength = 10 );
    void update( const vector<cv::Point2f> &detections );
    vector<Tracker> tracks;
private:
    float dt;
    float accelNoiseMag;
    float distThres;
    int maximumAllowedSkippedFrames;
    int maxTraceLength;
    int NextTrackID;
};

#endif
