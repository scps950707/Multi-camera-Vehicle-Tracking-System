#include "Ctracker.hpp"

/* Tracker. Manage tracks. Create, remove, update. */
CTracker::CTracker(
    float dt_,
    float Accel_noise_mag_,
    float dist_thres_,
    int maximum_allowed_skipped_frames_,
    int max_trace_length_
)
    :
    dt( dt_ ),
    Accel_noise_mag( Accel_noise_mag_ ),
    dist_thres( dist_thres_ ),
    maximum_allowed_skipped_frames( maximum_allowed_skipped_frames_ ),
    max_trace_length( max_trace_length_ ),
    NextTrackID( 0 )
{

}

void CTracker::Update( const std::vector<cv::Point2f> &detections, const std::vector<cv::Rect> &rects, DistType distType )
{
    CV_Assert( detections.size() == rects.size() );

    /* If there is no tracks yet, then every cv::Point begins its own track. */
    if ( tracks.size() == 0 )
    {
        /* If no tracks yet */
        for ( uint i = 0; i < detections.size(); i++ )
        {
            tracks.push_back( std::make_unique<CTrack>( detections[i], rects[i], dt, Accel_noise_mag, NextTrackID++ ) );
        }
    }

    int N = tracks.size();
    int M = detections.size();

    vector<int> assignment;

    if ( !tracks.empty() )
    {
        vector<float> Cost( N * M );

        switch ( distType )
        {
        case CentersDist:
            for ( uint i = 0; i < tracks.size(); i++ )
            {
                for ( uint j = 0; j < detections.size(); j++ )
                {
                    Cost[i + j * N] = tracks[i]->CalcDist( detections[j] );
                }
            }
            break;

        case RectsDist:
            for ( uint i = 0; i < tracks.size(); i++ )
            {
                for ( uint j = 0; j < detections.size(); j++ )
                {
                    Cost[i + j * N] = tracks[i]->CalcDist( rects[j] );
                }
            }
            break;
        }

        /* Solving assignment problem (tracks and predictions of Kalman filter) */
        AssignmentProblemSolver APS;
        APS.Solve( Cost, N, M, assignment, AssignmentProblemSolver::optimal );

        /* clean assignment from pairs with large distance */
        for ( uint i = 0; i < assignment.size(); i++ )
        {
            if ( assignment[i] != -1 )
            {
                if ( Cost[i + assignment[i] * N] > dist_thres )
                {
                    assignment[i] = -1;
                    tracks[i]->skipped_frames = 1;
                }
            }
            else
            {
                // If track have no assigned detect, then increment skipped frames counter.
                tracks[i]->skipped_frames++;
            }
        }

        /* If track didn't get detects long time, remove it. */
        for ( uint i = 0; i < tracks.size(); i++ )
        {
            if ( tracks[i]->skipped_frames > maximum_allowed_skipped_frames )
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
            tracks.push_back( std::make_unique<CTrack>( detections[i], rects[i], dt, Accel_noise_mag, NextTrackID++ ) );
        }
    }

    /* Update Kalman Filters state */

    for ( uint i = 0; i < assignment.size(); i++ )
    {
        /* If track updated less than one time, than filter state is not correct. */

        /* If we have assigned detect, then update using its coordinates */
        if ( assignment[i] != -1 )
        {
            tracks[i]->skipped_frames = 0;
            tracks[i]->Update( detections[assignment[i]], rects[assignment[i]], true, max_trace_length );
        }
        /* if not continue using predictions */
        else
        {
            tracks[i]->Update( cv::Point2f(), cv::Rect(), false, max_trace_length );
        }
    }

}
