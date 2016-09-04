#ifndef __AVI_HPP__
#define __AVI_HPP__

#include "header.hpp"

class aviWriter
{
public:
    aviWriter();
    aviWriter( string path, int fps, cv::Size vidSiz );
    void operator << ( cv::Mat mat );
private:
    string path;
    int fps;
    cv::Size vidSiz;
    cv::VideoWriter writer;
};


#endif
