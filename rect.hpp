#ifndef __RECT_H__
#define __RECT_H__

#include "header.hpp"

class findRect
{
public:
    findRect();
    void update( cv::Mat &inputImg, cv::Mat &mask );
    vector<cv::Point> getRectCenters();
    vector<cv::Point2f> getRectCentersFloat();
    vector<cv::Rect> getRects();
    bool isBurstOrRecovery();
private:
    cv::Mat inputImg, mask;
    cv::Rect removeShadowRect ( cv::Rect rect );
    bool burstLight;
    bool recovery;
    int frameRecoveryCnt;
    vector<cv::Point> rectCenters;
    vector<cv::Point2f> rectCentersFloat;
    vector<cv::Rect> rects;
};

#endif
