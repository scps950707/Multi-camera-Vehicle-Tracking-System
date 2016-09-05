#ifndef __RECT_H__
#define __RECT_H__

#include "header.hpp"

class findRect
{
public:
    findRect();
    vector<cv::Rect> findBoundingRect( cv::Mat &inputImg, cv::Mat &mask );
private:
    cv::Mat inputImg, mask;
    cv::Rect removeShadowRect ( cv::Rect rect );
    bool burstLight;
    bool recovery;
    int frameRecoveryCnt;
};

#endif
