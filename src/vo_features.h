// #include "opencv2/calib3d/calib3d.hpp"
// #include "opencv2/video/tracking.hpp"
// #include "opencv2/imgproc/imgproc.hpp"
// #include "opencv2/highgui/highgui.hpp"
// #include "opencv2/features2d/features2d.hpp"
#include "opencv2/opencv.hpp"

#include <iostream>
#include <ctype.h>
#include <algorithm>
#include <iterator>
#include <vector>
#include <ctime>
#include <sstream>
#include <fstream>
#include <string>

using namespace cv;
using namespace std;

// This function is intended to remove points from the correspondence based on LK results.
void featureTracking(Mat img_1, Mat img_2, vector<Point2f>& points1, vector<Point2f>& points2, vector<uchar>& status)	{

  vector<float> err;
  Size winSize=Size(21,21);
  TermCriteria termcrit=TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01);

  calcOpticalFlowPyrLK(img_1, img_2, points1, points2, status, err, winSize, 3, termcrit, 0, 0.001);

  // remove points if the KLT tracking is failed or if they are outside of the image frame
  int indexCorrection = 0;
  for( int i=0; i<status.size(); i++) {
    Point2f pt = points2.at(i- indexCorrection);

    if ( (status.at(i) == 0) ||
      (pt.x<0) ||
      (pt.y<0))	{
      if((pt.x<0)||(pt.y<0))	{
        status.at(i) = 0;
     	}

     	points1.erase (points1.begin() + (i - indexCorrection));
     	points2.erase (points2.begin() + (i - indexCorrection));
     	indexCorrection++;
    }
  }
}

void featureDetection(Mat img_1, vector<Point2f>& points1)	{
  vector<KeyPoint> keypoints_1;
  int fast_threshold = 20;
  bool nonmaxSuppression = true;
  FAST(img_1, keypoints_1, fast_threshold, nonmaxSuppression);
  KeyPoint::convert(keypoints_1, points1, vector<int>());
}
