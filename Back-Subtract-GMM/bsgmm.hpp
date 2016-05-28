#include <opencv2/core/core.hpp>
const double pi = 3.142;
const double cthr = 0.00001;
const double alpha = 0.002;
const double cT = 0.05;
const double covariance0 = 11.0;
const double cf = 0.1;
const double cfbar = 1.0 - cf;
const double temp_thr = 9.0 * covariance0 * covariance0;
const double prune = -alpha * cT;
const double alpha_bar = 1.0 - alpha;

//Structure used for saving various components for each pixel
struct gaussian
{
  double mean[3], covariance;
  double weight;
  // Represents the measure to which a particular component defines the pixel value
  gaussian *Next;
  gaussian *Previous;
};
struct node
{
  gaussian *pixel_s;
  gaussian *pixel_r;
  int no_of_components;
  node *Next;
};
node *Create_Node( double info1, double info2, double info3 );
void Insert_End_Node( node *np );
gaussian *Create_gaussian( double info1, double info2, double info3 );
