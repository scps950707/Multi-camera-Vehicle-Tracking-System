#ifndef __KALMAN_HPP__
#define __KALMAN_HPP__

#include "header.hpp"

class TKalmanFilter
{
public:
    cv::KalmanFilter kalman;
    double deltatime;
    cv::Point2f LastResult;
    TKalmanFilter( cv::Point2f p, float dt = 0.2, float Accel_noise_mag = 0.5 );
    cv::Point2f GetPrediction();
    cv::Point2f Update( cv::Point2f p, bool DataCorrect );
};

#endif
