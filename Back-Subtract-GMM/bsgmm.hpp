#ifndef BSGMM_HPP
#define BSGMM_HPP

const double alpha = 0.002;
const double cT = 0.05;
const double defaultVariance = 11.0;
const double minVariance = 5.0;
const double maxVariance = 17.0;
const double cf = 0.1;
const double cfbar = 1.0 - cf;
const double prune = -alpha * cT;
const double alpha_bar = 1.0 - alpha;
const double BGSigma = 4 * 4;
const double closeSigma = 3 * 3;
const int defaultGMMCount = 4;

enum Color
{
  BLACK = 0,
  WHITE = 255
};

typedef struct
{
  double mean[3];
  double variance;
  double weight;
} gaussian, *gaussianPtr;

typedef struct
{
  gaussian arr[defaultGMMCount + 1];
  int GMMCount;
} NODE, *NODEPTR;

NODE Create_Node();
gaussian Create_gaussian( double r, double g, double b, double variance, double weight );
void freeMem();

#endif
