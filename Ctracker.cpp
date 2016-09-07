#include "Ctracker.hpp"

CTrack::CTrack( cv::Point2f pt, float dt, float Accel_noise_mag )
{
    KF = new TKalmanFilter( pt, dt, Accel_noise_mag );
    prediction = pt;
    skipped_frames = 0;
}
CTrack::~CTrack()
{
    delete KF;
}

CTracker::CTracker( float _dt, float _Accel_noise_mag, double _dist_thres, int _maximum_allowed_skipped_frames, int _max_trace_length )
{
    dt = _dt;
    Accel_noise_mag = _Accel_noise_mag;
    dist_thres = _dist_thres;
    maximum_allowed_skipped_frames = _maximum_allowed_skipped_frames;
    max_trace_length = _max_trace_length;
}

void CTracker::Update( vector<cv::Point2d> &detections )
{
    if ( tracks.size() == 0 )
    {
        for ( uint i = 0; i < detections.size(); i++ )
        {
            tracks.push_back( new CTrack( detections[i], dt, Accel_noise_mag ) );
        }
    }

    int N = tracks.size();
    int M = detections.size();
    vector<vector<double>> Cost( N, vector<double>( M ) );
    vector<int> assignment;

    double dist;
    for ( uint i = 0; i < tracks.size(); i++ )
    {
        for ( uint j = 0; j < detections.size(); j++ )
        {
            cv::Point2d diff = ( tracks[i]->prediction - detections[j] );
            dist = sqrtf( diff.x * diff.x + diff.y * diff.y );
            Cost[i][j] = dist;
        }
    }
    AssignmentProblemSolver APS;
    APS.Solve( Cost, assignment, AssignmentProblemSolver::optimal );
    vector<int> not_assigned_tracks;

    for ( uint i = 0; i < assignment.size(); i++ )
    {
        if ( assignment[i] != -1 )
        {
            if ( Cost[i][assignment[i]] > dist_thres )
            {
                assignment[i] = -1;
                not_assigned_tracks.push_back( i );
            }
        }
        else
        {
            tracks[i]->skipped_frames++;
        }

    }

    for ( uint i = 0; i < tracks.size(); i++ )
    {
        if ( ( int )tracks[i]->skipped_frames > maximum_allowed_skipped_frames )
        {
            tracks.erase( tracks.begin() + i );
            assignment.erase( assignment.begin() + i );
            i--;
        }
    }
    vector<int> not_assigned_detections;
    vector<int>::iterator it;
    for ( uint i = 0; i < detections.size(); i++ )
    {
        it = find( assignment.begin(), assignment.end(), i );
        if ( it == assignment.end() )
        {
            not_assigned_detections.push_back( i );
        }
    }

    if ( not_assigned_detections.size() != 0 )
    {
        for ( uint i = 0; i < not_assigned_detections.size(); i++ )
        {
            tracks.push_back( new CTrack( detections[not_assigned_detections[i]], dt, Accel_noise_mag ) );
        }
    }


    for ( uint i = 0; i < assignment.size(); i++ )
    {
        tracks[i]->KF->GetPrediction();
        if ( assignment[i] != -1 )
        {
            tracks[i]->skipped_frames = 0;
            tracks[i]->prediction = tracks[i]->KF->Update( detections[assignment[i]], 1 );
        }
        else
        {
            tracks[i]->prediction = tracks[i]->KF->Update( cv::Point2f( 0, 0 ), 0 );
        }

        if ( ( int )tracks[i]->trace.size() > max_trace_length )
        {
            tracks[i]->trace.erase( tracks[i]->trace.begin(), tracks[i]->trace.end() - max_trace_length );
        }

        tracks[i]->trace.push_back( tracks[i]->prediction );
        tracks[i]->KF->LastResult = tracks[i]->prediction;
    }

}

CTracker::~CTracker( void )
{
    for ( uint i = 0; i < tracks.size(); i++ )
    {
        delete tracks[i];
    }
    tracks.clear();
}
