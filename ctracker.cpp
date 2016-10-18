#include "ctracker.hpp"

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

/* void Tracker::Update( const cv::Point2f &p, bool dataCorrect, int maxTraceLength ) {{{*/
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

/* MultiTracker::MultiTracker(...) {{{*/
/* Tracker. Manage tracks. Create, remove, update. */
MultiTracker::MultiTracker(
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

/* void MultiTracker::update( const vector<cv::Point2f> &detections ) {{{*/
void MultiTracker::update( const vector<cv::Point2f> &detections )
{
    /* If there is no tracks yet, then every cv::Point begins its own track. */
    if ( tracks.size() == 0 )
    {
        /* If no tracks yet */
        for ( uint i = 0; i < detections.size(); i++ )
        {
            tracks.push_back( Tracker( detections[i], dt, accelNoiseMag, NextTrackID++ ) );
        }
    }

    int N = tracks.size();
    int M = detections.size();

    vector<int> assignment;

    if ( tracks.empty() == false )
    {
        vector<float> Cost( N * M );

        for ( uint i = 0; i < tracks.size(); i++ )
        {
            for ( uint j = 0; j < detections.size(); j++ )
            {
                Cost[i + j * N] = tracks[i].calcDist( detections[j] );
            }
        }

        /* Solving assignment problem (tracks and predictions of Kalman filter) */
        assignmentProblemSolver APS( N, M );
        APS.Solve( Cost );
        assignment = APS.getAssignment();

        /* clean assignment from pairs with large distance */
        for ( uint i = 0; i < assignment.size(); i++ )
        {
            if ( assignment[i] != -1 )
            {
                if ( Cost[i + assignment[i] * N] > distThres )
                {
                    assignment[i] = -1;
                    tracks[i].skippedFrames = 1;
                }
            }
            else
            {
                // If track have no assigned detect, then increment skipped frames counter.
                tracks[i].skippedFrames++;
            }
        }

        /* If track didn't get detects long time, remove it. */
        for ( uint i = 0; i < tracks.size(); i++ )
        {
            if ( tracks[i].skippedFrames > maximumAllowedSkippedFrames )
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
            tracks.push_back( Tracker( detections[i], dt, accelNoiseMag, NextTrackID++ ) );
        }
    }

    /* Update Kalman Filters state */

    for ( uint i = 0; i < assignment.size(); i++ )
    {
        /* If track updated less than one time, than filter state is not correct. */

        /* If we have assigned detect, then update using its coordinates */
        if ( assignment[i] != -1 )
        {
            tracks[i].skippedFrames = 0;
            tracks[i].update( detections[assignment[i]], true, maxTraceLength );
        }
        /* if not continue using predictions */
        else
        {
            tracks[i].update( cv::Point2f(), false, maxTraceLength );
        }
    }

}
/* }}} */
