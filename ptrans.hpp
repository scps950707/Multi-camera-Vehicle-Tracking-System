#ifndef __PTRANS_HPP__
#define __PTRANS_HPP__

#include "header.hpp"

class perspectiveTransform
{
public:
    perspectiveTransform();
    void setSrcPts( cv::Point2f srcTl, cv::Point2f srcBl, cv::Point2f srcBr, cv::Point2f srcTr );
    void setDstPts( cv::Point2f dstTl, cv::Point2f dstBl, cv::Point2f dstBr, cv::Point2f dstTr );
    cv::Mat getMatrix();
    cv::Point2f *getSrcPts();
    cv::Point2f getSrcTl();
    cv::Point2f getSrcBl();
    cv::Point2f getSrcBr();
    cv::Point2f getSrcTr();
    cv::Point2f *getDstPts();
    cv::Point2f getDstTl();
    cv::Point2f getDstBl();
    cv::Point2f getDstBr();
    cv::Point2f getDstTr();
private:
    cv::Point2f srcPts[4], dstPts[4];
};

#endif
