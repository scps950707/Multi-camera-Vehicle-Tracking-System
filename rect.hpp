#ifndef __RECT_H__
#define __RECT_H__

#include "header.hpp"

class findRect
{
public:
    findRect( cv::Mat &inputImg, cv::Mat &mask );
    vector<cv::Rect> findBoundingRect();
private:
    cv::Mat inputImg, mask;
    cv::Rect removeShadowRect ( cv::Rect rect );
};

#endif
