#include "Kalman.hpp"
#include "HungarianAlg.hpp"
#include "header.hpp"
#include <memory>

class CTrack
{
public:
    CTrack( const cv::Point2f &p, const cv::Rect &rect, float dt, float Accel_noise_mag, int trackID )
        :
        track_id( trackID ),
        skipped_frames( 0 ),
        prediction( p ),
        lastRect( rect ),
        KF( p, dt, Accel_noise_mag )
    {
    }

    float CalcDist( const cv::Point2f &p )
    {
        cv::Point2f diff = prediction - p;
        return sqrtf( diff.x * diff.x + diff.y * diff.y );
    }

    float CalcDist( const cv::Rect &r )
    {
        std::array<float, 4> diff;
        diff[0] = prediction.x - lastRect.width / 2 - r.x;
        diff[1] = prediction.y - lastRect.height / 2 - r.y;
        diff[2] = static_cast<float>( lastRect.width - r.width );
        diff[3] = static_cast<float>( lastRect.height - r.height );

        float dist = 0;
        for ( uint i = 0; i < diff.size(); i++ )
        {
            dist += diff[i] * diff[i];
        }
        return sqrtf( dist );
    }

    void Update( const cv::Point2f &p, const cv::Rect &rect, bool dataCorrect, int max_trace_length )
    {
        KF.GetPrediction();
        prediction = KF.Update( p, dataCorrect );

        if ( dataCorrect )
        {
            lastRect = rect;
        }

        if ( trace.size() > ( uint )max_trace_length )
        {
            trace.erase( trace.begin(), trace.end() - max_trace_length );
        }

        trace.push_back( prediction );
    }

    std::vector<cv::Point2f> trace;
    int track_id;
    int skipped_frames;

    cv::Rect GetLastRect()
    {
        return cv::Rect(
                   static_cast<int>( prediction.x - lastRect.width / 2 ),
                   static_cast<int>( prediction.y - lastRect.height / 2 ),
                   lastRect.width,
                   lastRect.height );
    }

private:
    cv::Point2f prediction;
    cv::Rect lastRect;
    TKalmanFilter KF;
};

class CTracker
{
public:
    CTracker( float dt_, float Accel_noise_mag_, float dist_thres_ = 60, int maximum_allowed_skipped_frames_ = 10, int max_trace_length_ = 10 );

    enum DistType
    {
        CentersDist = 0,
        RectsDist = 1
    };

    std::vector<std::unique_ptr<CTrack>> tracks;
    void Update( const std::vector<cv::Point2f> &detections, const std::vector<cv::Rect> &rects, DistType distType );

private:
    float dt;
    float Accel_noise_mag;
    float dist_thres;
    int maximum_allowed_skipped_frames;
    int max_trace_length;
    int NextTrackID;
};

