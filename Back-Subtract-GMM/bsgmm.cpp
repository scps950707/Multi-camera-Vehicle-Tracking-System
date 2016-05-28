#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/features2d/features2d.hpp>
#include <stdlib.h>
#include "bsgmm.hpp"
#include <vector>
using namespace std;
//Temperory variable
int overall = 0;
gaussian *ptr, *head, *tail,  *temp_ptr;
vector<NODE> NodeList;
vector<NODE>::iterator nodeIter;
//Some function associated with the structure management
NODE Create_Node( double info1, double info2, double info3 )
{
  NODE tmp;
  tmp.no_of_components = 1;
  tmp.pixel_r = tmp.pixel_s = Create_gaussian( info1, info2, info3 );
  return tmp;
}
gaussian *Create_gaussian( double info1, double info2, double info3 )
{
  ptr = new gaussian;
  if ( ptr != NULL )
  {
    ptr->mean[0] = info1;
    ptr->mean[1] = info2;
    ptr->mean[2] = info3;
    ptr->covariance = covariance0;
    ptr->weight = alpha;
    ptr->next = NULL;
    ptr->prev = NULL;
  }
  return ptr;
}
void Insert_End_gaussian( gaussian *nptr )
{
  if ( head != NULL )
  {
    tail->next = nptr;
    nptr->prev = tail;
    tail = nptr;
  }
  else
  {
    head = tail = nptr;
  }
}
gaussian *Delete_gaussian( gaussian *nptr )
{
  gaussian *previous, *nextptr;
  previous = nptr->prev;
  nextptr = nptr->next;
  if ( head != NULL )
  {
    if ( nptr == head && nptr == tail )
    {
      head = tail = NULL;
      delete nptr;
    }
    else if ( nptr == head )
    {
      nextptr->prev = NULL;
      head = nextptr;
      delete nptr;
      nptr = head;
    }
    else if ( nptr == tail )
    {
      previous->next = NULL;
      tail = previous;
      delete nptr;
      nptr = tail;
    }
    else
    {
      previous->next = nextptr;
      nextptr->prev = previous;
      delete nptr;
      nptr = nextptr;
    }
  }
  else
  {
    std::cout << "Underflow........";
    exit( 0 );
  }
  return nptr;
}
int main( int argc, char *argv[] )
{
  // Declare matrices to store original and resultant binary image
  cv::Mat orig_img, bin_img;
  //Declare a VideoCapture object to store incoming frame and initialize it
  cv::VideoCapture capture( argv[1] );
  //Checking if input source is valid
  if ( !capture.read( orig_img ) )
  {
    std::cout << " Can't recieve input from source ";
    exit( EXIT_FAILURE );
  }
  cv::cvtColor( orig_img, orig_img, CV_BGR2YCrCb );
  //Initializing the binary image with the same dimensions as original image
  bin_img = cv::Mat( orig_img.rows, orig_img.cols, CV_8U, cv::Scalar( 0 ) );
  cv::Vec3f val;
  uchar *r_ptr;
  uchar *b_ptr;
  for ( int i = 0; i < orig_img.rows; i++ )
  {
    r_ptr = orig_img.ptr( i );
    for ( int j = 0; j < orig_img.cols; j++ )
    {
      NODE tmp = Create_Node( *r_ptr, *( r_ptr + 1 ), *( r_ptr + 2 ) );
      tmp.pixel_s->weight = 1.0;
      NodeList.push_back( tmp );
    }
  }
  capture.read( orig_img );
  int nL, nC;
  if ( orig_img.isContinuous() == true )
  {
    nL = 1;
    nC = orig_img.rows * orig_img.cols * orig_img.channels();
  }
  else
  {
    nL = orig_img.rows;
    nC = orig_img.cols * orig_img.channels();
  }
  //Step 2: Modelling each pixel with Gaussian
  bin_img = cv::Mat( orig_img.rows, orig_img.cols, CV_8UC1, cv::Scalar( 0 ) );
  while ( 1 )
  {
    double mal_dist;
    double sum = 0.0;
    bool close = false;
    int background;
    double mult;
    double temp_cov = 0.0;
    double weight = 0.0;
    double var = 0.0;
    double muR, muG, muB, dR, dG, dB, rVal, gVal, bVal;
    capture.read( orig_img );
    nodeIter = NodeList.begin();
    for ( int i = 0; i < nL; i++ )
    {
      r_ptr = orig_img.ptr( i );
      b_ptr = bin_img.ptr( i );
      for ( int j = 0; j < nC; j += 3 )
      {
        sum = 0.0;
        close = false;
        background = 0;
        rVal = *( r_ptr++ );
        gVal = *( r_ptr++ );
        bVal = *( r_ptr++ );
        head = nodeIter->pixel_s;
        tail = nodeIter->pixel_r;
        ptr = head;
        temp_ptr = NULL;
        if ( nodeIter->no_of_components > 4 )
        {
          Delete_gaussian( tail );
          nodeIter->no_of_components--;
        }
        for ( int k = 0; k < nodeIter->no_of_components; k++ )
        {
          weight = ptr->weight;
          mult = alpha / weight;
          weight = weight * alpha_bar + prune;
          if ( close == false )
          {
            muR = ptr->mean[0];
            muG = ptr->mean[1];
            muB = ptr->mean[2];
            dR = rVal - muR;
            dG = gVal - muG;
            dB = bVal - muB;
            var = ptr->covariance;
            mal_dist = ( dR * dR + dG * dG + dB * dB );
            if ( ( sum < cfbar ) && ( mal_dist < 16.0 * var * var ) )
            {
              background = 255;
            }
            if ( mal_dist < 9.0 * var * var )
            {
              weight += alpha;
              close = true;
              ptr->mean[0] = muR + mult * dR;
              ptr->mean[1] = muG + mult * dG;
              ptr->mean[2] = muB + mult * dB;
              temp_cov = var + mult * ( mal_dist - var );
              ptr->covariance = temp_cov < 5.0 ? 5.0 : ( temp_cov > 20.0 ? 20.0 : temp_cov );
              temp_ptr = ptr;
            }
          }
          if ( weight < -prune )
          {
            ptr = Delete_gaussian( ptr );
            weight = 0;
            nodeIter->no_of_components--;
          }
          else
          {
            sum += weight;
            ptr->weight = weight;
          }
          ptr = ptr->next;
        }
        if ( close == false )
        {
          ptr = new gaussian;
          ptr->weight = alpha;
          ptr->mean[0] = rVal;
          ptr->mean[1] = gVal;
          ptr->mean[2] = bVal;
          ptr->covariance = covariance0;
          ptr->next = NULL;
          ptr->prev = NULL;
          if ( head == NULL )
          {
            head = tail = NULL;
          }
          else
          {
            ptr->prev = tail;
            tail->next = ptr;
            tail = ptr;
          }
          temp_ptr = ptr;
          nodeIter->no_of_components++;
        }
        ptr = head;
        while ( ptr != NULL )
        {
          ptr->weight /= sum;
          ptr = ptr->next;
        }
        while ( temp_ptr != NULL && temp_ptr->prev != NULL )
        {
          if ( temp_ptr->weight <= temp_ptr->prev->weight )
          {
            break;
          }
          else
          {
            gaussian *nextptr, *previous;
            nextptr = temp_ptr->next;
            previous = temp_ptr->prev;
            if ( head == previous )
            {
              head = temp_ptr;
            }
            previous->next = nextptr;
            temp_ptr->prev = previous->prev;
            temp_ptr->next = previous;
            if ( previous->prev != NULL )
            {
              previous->prev->next = temp_ptr;
            }
            if ( nextptr != NULL )
            {
              nextptr->prev = previous;
            }
            else
            {
              tail = previous;
            }
            previous->prev = temp_ptr;
          }
          temp_ptr = temp_ptr->prev;
        }
        nodeIter->pixel_s = head;
        nodeIter->pixel_r = tail;
        *b_ptr++ = background;
        nodeIter++;
      }
    }
    cv::imshow( "video", orig_img );
    cv::imshow( "gp", bin_img );
    if ( cv::waitKey( 1 ) > 0 )
    {
      break;
    }
  }
  return 0;
}
