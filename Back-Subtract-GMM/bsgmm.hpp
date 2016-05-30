#include <opencv2/core/core.hpp>
const double pi = 3.142;
const double cthr = 0.00001;
const double alpha = 0.002;
const double cT = 0.05;
const double defaultCovariance = 11.0;
const double cf = 0.1;
const double cfbar = 1.0 - cf;
const double temp_thr = 9.0 * defaultCovariance * defaultCovariance;
const double prune = -alpha * cT;
const double alpha_bar = 1.0 - alpha;

typedef struct tmp
{
  double mean[3], covariance;
  double weight;
  tmp *next;
  tmp *prev;
} gaussian, *gaussianPtr;
typedef struct
{
  gaussian *pixelCompoListHead;
  gaussian *pixelCompoListTail;
  int numComponents;
} NODE, *NODEPTR;
NODE Create_Node( double r, double g, double b, double covariance, double weight );
gaussianPtr Create_gaussian( double r, double g, double b, double covariance, double weight );
void insertBack( gaussianPtr *head, gaussianPtr *tail, gaussianPtr newPtr );
gaussianPtr popBack( gaussianPtr *head, gaussianPtr *tail, gaussianPtr target );
void swapNode( gaussianPtr *head, gaussianPtr *tail, gaussianPtr p1, gaussianPtr p2 );
