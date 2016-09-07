#include "Kalman.hpp"
#include "HungarianAlg.hpp"
#include "header.hpp"

class CTrack
{
public:
    vector<cv::Point2d> trace;
    static size_t NextTrackID;
    size_t skipped_frames;
    cv::Point2d prediction;
    TKalmanFilter *KF;
    CTrack( cv::Point2f p, float dt, float Accel_noise_mag );
    ~CTrack();
};


class CTracker
{
public:
    float dt;
    float Accel_noise_mag;
    double dist_thres;
    int maximum_allowed_skipped_frames;
    int max_trace_length;
    vector<CTrack *> tracks;
    void Update( vector<cv::Point2d> &detections );
    CTracker( float _dt, float _Accel_noise_mag, double _dist_thres = 60, int _maximum_allowed_skipped_frames = 10, int _max_trace_length = 10 );
    ~CTracker( void );
};
