#include "Kalman.hpp"
#include "HungarianAlg.hpp"
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
    CTracker( float dt_, float Accel_noise_mag_, float dist_thres_ = 60, int maximum_allowed_skipped_frames_ = 10, int max_trace_length_ = 10 );
    std::vector<std::unique_ptr<CTrack>> tracks;
    void Update( const std::vector<cv::Point2f> &detections );
private:
    float dt;
    float accelNoiseMag;
    float distThres;
    int maximumAllowedSkippedFrames;
    int maxTraceLength;
    int NextTrackID;
};

