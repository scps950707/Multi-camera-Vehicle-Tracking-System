#ifndef __RECT_H__
#define __RECT_H__

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;

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
