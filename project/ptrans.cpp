#include "ptrans.hpp"

perspectiveTransform::perspectiveTransform()
{
    for ( int i = 0; i < 4; i++ )
    {
        this->srcPts[i] = cv::Point2f( -1, -1 );
        this->dstPts[i] = cv::Point2f( -1, -1 );
    }
}

void perspectiveTransform::setSrcPts( cv::Point2f srcTl, cv::Point2f srcBl, cv::Point2f srcBr, cv::Point2f srcTr )
{
    srcPts[0] = srcTl;
    srcPts[1] = srcBl;
    srcPts[2] = srcBr;
    srcPts[3] = srcTr;
}

void perspectiveTransform::setDstPts( cv::Point2f dstTl, cv::Point2f dstBl, cv::Point2f dstBr, cv::Point2f dstTr )
{
    dstPts[0] = dstTl;
    dstPts[1] = dstBl;
    dstPts[2] = dstBr;
    dstPts[3] = dstTr;
}

cv::Mat perspectiveTransform::getMatrix()
{
    return cv::getPerspectiveTransform( srcPts, dstPts );
}
