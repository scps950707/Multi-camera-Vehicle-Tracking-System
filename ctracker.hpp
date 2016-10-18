#include "kalman.hpp"
#include "hungarianAlg.hpp"
#include "header.hpp"
#include <memory>

class CTrack
{
public:
    CTrack( const cv::Point2f &p, float dt, float accelNoiseMag, int trackID );
    float CalcDist( const cv::Point2f &p );
    void Update( const cv::Point2f &p, bool dataCorrect, int maxTraceLength );
    std::vector<cv::Point2f> trace;
    int trackId;
    int skippedFrames;
private:
    cv::Point2f prediction;
    TKalmanFilter KF;
};

class CTracker
{
public:
    CTracker( float dt, float accelNoiseMag, float distThres = 60, int maxiumAllowedSkippedFrames = 10, int maxTraceLength = 10 );
    std::vector<CTrack> tracks;
    void Update( const std::vector<cv::Point2f> &detections );
private:
    float dt;
    float accelNoiseMag;
    float distThres;
    int maximumAllowedSkippedFrames;
    int maxTraceLength;
    int NextTrackID;
};

