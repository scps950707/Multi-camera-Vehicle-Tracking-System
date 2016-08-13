#ifndef __STANDARD_HPP__
#define __STANDARD_HPP__

#include "header.hpp"

template<typename T>
class StandardDeviation
{
public:
    StandardDeviation( vector<cv::Point> pts );
    vector<T> normalize( vector<T> target, int xRange, int yRange );
    double getxAvg();
    double getxSigma();
    double getyAvg();
    double getySigma();
private:
    vector<cv::Point> pts;
    double xAvg;
    double xSigma;
    double yAvg;
    double ySigma;
};

template<typename T>
StandardDeviation<T>::StandardDeviation( vector<cv::Point> pts )
{
    this->pts = pts;
    for ( unsigned int i = 0; i < pts.size(); i++ )
    {
        xAvg += pts[i].x;
        yAvg += pts[i].y;
    }
    xAvg /= pts.size();
    yAvg /= pts.size();
    for ( unsigned int i = 0; i < pts.size(); i++ )
    {
        xSigma += pow( pts[i].x - xAvg, 2 );
        ySigma += pow( pts[i].y - yAvg, 2 );
    }
    xSigma /= pts.size();
    xSigma = sqrt( xSigma );
    ySigma /= pts.size();
    ySigma = sqrt( ySigma );
}

template<typename T>
vector<T> StandardDeviation<T>::normalize( vector<T> target, int xRange, int yRange )
{
    vector<T> res;
    for ( unsigned int i = 0; i < target.size(); i++ )
    {
        if ( pts[i].x > xAvg - xRange * xSigma
                && pts[i].x < xAvg + xRange * xSigma
                && pts[i].y > yAvg - yRange * ySigma
                && pts[i].y < yAvg + yRange * ySigma )
        {
            res.push_back( target[i] );
        }
    }
    return res;
}

template<typename T>
double StandardDeviation<T>::getxAvg()
{
    return this->xAvg;
}

template<typename T>
double StandardDeviation<T>::getyAvg()
{
    return this->yAvg;
}

template<typename T>
double StandardDeviation<T>::getxSigma()
{
    return this->xSigma;
}

template<typename T>
double StandardDeviation<T>::getySigma()
{
    return this->ySigma;
}

#endif
