#include "header.hpp"
#include "avi.hpp"

int main( int argc, char *argv[] )
{
    if ( argc != 5 )
    {
        cout << "./resizer input_path output_path width height" << endl;
        exit( EXIT_FAILURE );
    }
    cv::VideoCapture cap( argv[1] );
    cv::Mat old, newv;
    cv::Size newSize( atoi( argv[3] ), atoi( argv[4] ) );
    aviWriter wr( argv[2], 30, newSize );
    cout << "total frames:" << ( int )cap.get( CV_CAP_PROP_FRAME_COUNT ) << endl;
    while ( cap.read( old ) )
    {
        int cur_frame_id = cap.get( CV_CAP_PROP_POS_FRAMES );
        if ( cur_frame_id % 500 == 0 )
        {
            cout << "processing frames:" << cur_frame_id << endl;
        }
        cv::resize( old, newv, newSize );
        wr << newv;
        if ( cv::waitKey( 1 ) > 0 )
        {
            break;
        }
    }
    return 0;
}
