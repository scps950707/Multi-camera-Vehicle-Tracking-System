#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
using namespace cv;
int main( int argc, char *argv[] )
{
  Mat frame;
  Mat fgMaskMOG;
  Mat fgMaskMOG2;
  Mat fgMaskGMG;
  Ptr< BackgroundSubtractor> pMOG = new BackgroundSubtractorMOG(); //MOG Background subtractor
  Ptr< BackgroundSubtractor> pMOG2 = new BackgroundSubtractorMOG2(); //MOG2 Background subtractor
  Ptr< BackgroundSubtractorGMG> pGMG = new BackgroundSubtractorGMG(); //MOG2 Background subtractor
  VideoCapture capture( argv[1] );
  while ( capture.read( frame ) )
  {
    pMOG->operator()( frame, fgMaskMOG );
    pMOG2->operator()( frame, fgMaskMOG2 );
    pGMG->operator()( frame, fgMaskGMG );
    imshow( "orgin", frame );
    imshow( "MOG", fgMaskMOG );
    imshow( "MOG2", fgMaskMOG2 );
    imshow( "GMG", fgMaskGMG );
    if ( waitKey( 1 ) > 0 )
    {
      break;
    }
  }
}
