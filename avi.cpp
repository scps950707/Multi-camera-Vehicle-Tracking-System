#include "avi.hpp"

/* aviWriter::aviWriter() {{{*/
aviWriter::aviWriter()
{

}
/* }}} */

/* aviWriter::aviWriter( string path, int fps, cv::Size vidSiz ) {{{*/
aviWriter::aviWriter( string path, int fps, cv::Size vidSiz )
{
    this->path = path;
    this->fps = fps;
    this->vidSiz = vidSiz;
    this->writer.open( path, CV_FOURCC( 'D', 'I', 'V', 'X' ), fps, vidSiz );
}
/* }}} */

/* void aviWriter::operator << ( cv::Mat mat ) {{{*/
void aviWriter::operator << ( cv::Mat mat )
{
    CV_Assert( mat.size() == this->vidSiz );
    if ( mat.channels() == 1 )
    {
        cv::cvtColor( mat, mat, CV_GRAY2RGB );
    }
    this->writer << mat;
}
/* }}} */
