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
private:
    cv::Point2f srcPts[4], dstPts[4];
};

#endif
