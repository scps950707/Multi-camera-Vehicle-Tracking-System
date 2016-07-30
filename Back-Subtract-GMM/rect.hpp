#ifndef __RECT_H__
#define __RECT_H__

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;

class findRect
{
public:
    void findBoundingRect( cv::Mat target, cv::Mat mask );
private:
    int containTargetPerCent( cv::Rect container, cv::Rect target );
};

#endif
