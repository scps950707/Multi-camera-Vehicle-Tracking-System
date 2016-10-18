#ifndef __MULTITRACKER_HPP__
#define __MULTITRACKER_HPP__

#include "kalman.hpp"
#include "hungarianAlg.hpp"
#include "header.hpp"
#include "tracker.hpp"
#include <memory>

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
