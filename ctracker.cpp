#include "ctracker.hpp"

/* CTrack::CTrack( const cv::Point2f &p, float dt, float accelNoiseMag, int trackID ) {{{*/
CTrack::CTrack( const cv::Point2f &p, float dt, float accelNoiseMag, int trackID )
    :
    trackId( trackID ),
    skippedFrames( 0 ),
    prediction( p ),
    KF( p, dt, accelNoiseMag )
{
}
/* }}} */

/* float CTrack::CalcDist( const cv::Point2f &p ) {{{*/
float CTrack::CalcDist( const cv::Point2f &p )
{
    cv::Point2f diff = prediction - p;
    return sqrtf( diff.x * diff.x + diff.y * diff.y );
}
/* }}} */

/* void CTrack::Update( const cv::Point2f &p, bool dataCorrect, int maxTraceLength ) {{{*/
void CTrack::Update( const cv::Point2f &p, bool dataCorrect, int maxTraceLength )
{
    KF.GetPrediction();
    prediction = KF.Update( p, dataCorrect );

    if ( trace.size() > ( uint )maxTraceLength )
    {
        trace.erase( trace.begin(), trace.end() - maxTraceLength );
    }

    trace.push_back( prediction );
}
/* }}} */

/* CTracker::CTracker(...) {{{*/
/* Tracker. Manage tracks. Create, remove, update. */
CTracker::CTracker(
    float dt,
    float accelNoiseMag,
    float distThres,
    int maximumAllowedSkippedFrames,
    int maxTraceLength
)
    :
    dt( dt ),
    accelNoiseMag( accelNoiseMag ),
    distThres( distThres ),
    maximumAllowedSkippedFrames( maximumAllowedSkippedFrames ),
    maxTraceLength( maxTraceLength )
{
    this->NextTrackID = 0;
}
/* }}} */

/* void CTracker::Update( const std::vector<cv::Point2f> &detections ) {{{*/
void CTracker::Update( const std::vector<cv::Point2f> &detections )
{
    /* If there is no tracks yet, then every cv::Point begins its own track. */
    if ( tracks.size() == 0 )
    {
        /* If no tracks yet */
        for ( uint i = 0; i < detections.size(); i++ )
        {
            tracks.push_back( std::make_unique<CTrack>( detections[i], dt, accelNoiseMag, NextTrackID++ ) );
        }
    }

    int N = tracks.size();
    int M = detections.size();

    vector<int> assignment;

    if ( !tracks.empty() )
    {
        vector<float> Cost( N * M );

        for ( uint i = 0; i < tracks.size(); i++ )
        {
            for ( uint j = 0; j < detections.size(); j++ )
            {
                Cost[i + j * N] = tracks[i]->CalcDist( detections[j] );
            }
        }

        /* Solving assignment problem (tracks and predictions of Kalman filter) */
        AssignmentProblemSolver APS;
        APS.Solve( Cost, N, M, assignment, AssignmentProblemSolver::optimal );

        /* clean assignment from pairs with large distance */
        for ( uint i = 0; i < assignment.size(); i++ )
        {
            if ( assignment[i] != -1 )
            {
                if ( Cost[i + assignment[i] * N] > distThres )
                {
                    assignment[i] = -1;
                    tracks[i]->skippedFrames = 1;
                }
            }
            else
            {
                // If track have no assigned detect, then increment skipped frames counter.
                tracks[i]->skippedFrames++;
            }
        }

        /* If track didn't get detects long time, remove it. */
        for ( uint i = 0; i < tracks.size(); i++ )
        {
            if ( tracks[i]->skippedFrames > maximumAllowedSkippedFrames )
            {
                tracks.erase( tracks.begin() + i );
                assignment.erase( assignment.begin() + i );
                i--;
            }
        }
    }

    /* Search for unassigned detects and start new tracks for them. */
    for ( uint i = 0; i < detections.size(); ++i )
    {
        if ( find( assignment.begin(), assignment.end(), i ) == assignment.end() )
        {
            tracks.push_back( std::make_unique<CTrack>( detections[i], dt, accelNoiseMag, NextTrackID++ ) );
        }
    }

    /* Update Kalman Filters state */

    for ( uint i = 0; i < assignment.size(); i++ )
    {
        /* If track updated less than one time, than filter state is not correct. */

        /* If we have assigned detect, then update using its coordinates */
        if ( assignment[i] != -1 )
        {
            tracks[i]->skippedFrames = 0;
            tracks[i]->Update( detections[assignment[i]], true, maxTraceLength );
        }
        /* if not continue using predictions */
        else
        {
            tracks[i]->Update( cv::Point2f(), false, maxTraceLength );
        }
    }

}
/* }}} */
