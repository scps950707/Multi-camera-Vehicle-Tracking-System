#include "ptrans.hpp"

/* perspectiveTransform::perspectiveTransform() {{{*/
perspectiveTransform::perspectiveTransform()
{
    for ( int i = 0; i < 4; i++ )
    {
        this->srcPts[i] = cv::Point2f( -1, -1 );
        this->dstPts[i] = cv::Point2f( -1, -1 );
    }
}
/* }}} */

/* void perspectiveTransform::setSrcPts( cv::Point2f srcTl, cv::Point2f srcBl, cv::Point2f srcBr, cv::Point2f srcTr ) {{{*/
void perspectiveTransform::setSrcPts( cv::Point2f srcTl, cv::Point2f srcBl, cv::Point2f srcBr, cv::Point2f srcTr )
{
    srcPts[0] = srcTl;
    srcPts[1] = srcBl;
    srcPts[2] = srcBr;
    srcPts[3] = srcTr;
}
/* }}} */

/* void perspectiveTransform::setDstPts( cv::Point2f dstTl, cv::Point2f dstBl, cv::Point2f dstBr, cv::Point2f dstTr ) {{{*/
void perspectiveTransform::setDstPts( cv::Point2f dstTl, cv::Point2f dstBl, cv::Point2f dstBr, cv::Point2f dstTr )
{
    dstPts[0] = dstTl;
    dstPts[1] = dstBl;
    dstPts[2] = dstBr;
    dstPts[3] = dstTr;
}
/* }}} */

/* cv::Mat perspectiveTransform::getMatrix() {{{*/
cv::Mat perspectiveTransform::getMatrix()
{
    return cv::getPerspectiveTransform( srcPts, dstPts );
}
/* }}} */

/* cv::Point2f *perspectiveTransform::getSrcPts() {{{*/
cv::Point2f *perspectiveTransform::getSrcPts()
{
    return this->srcPts;
}
/* }}} */

/* cv::Point2f *perspectiveTransform::getDstPts() {{{*/
cv::Point2f *perspectiveTransform::getDstPts()
{
    return this->dstPts;
}
/* }}} */

/* cv::Point2f perspectiveTransform::getSrcTl() {{{*/
cv::Point2f perspectiveTransform::getSrcTl()
{
    return srcPts[0];
}
/* }}} */

/* cv::Point2f perspectiveTransform::getSrcBl() {{{*/
cv::Point2f perspectiveTransform::getSrcBl()
{
    return srcPts[1];
}
/* }}} */

/* cv::Point2f perspectiveTransform::getSrcBr() {{{*/
cv::Point2f perspectiveTransform::getSrcBr()
{
    return srcPts[2];
}
/* }}} */

/* cv::Point2f perspectiveTransform::getSrcTr() {{{*/
cv::Point2f perspectiveTransform::getSrcTr()
{
    return srcPts[3];
}
/* }}} */

/* cv::Point2f perspectiveTransform::getDstTl() {{{*/
cv::Point2f perspectiveTransform::getDstTl()
{
    return dstPts[0];
}
/* }}} */

/* cv::Point2f perspectiveTransform::getDstBl() {{{*/
cv::Point2f perspectiveTransform::getDstBl()
{
    return dstPts[1];
}
/* }}} */

/* cv::Point2f perspectiveTransform::getDstBr() {{{*/
cv::Point2f perspectiveTransform::getDstBr()
{
    return dstPts[2];
}
/* }}} */

/* cv::Point2f perspectiveTransform::getDstTr() {{{*/
cv::Point2f perspectiveTransform::getDstTr()
{
    return dstPts[3];
}
/* }}} */
