#include "vo_features.h"

using namespace cv;
using namespace std;

#define MAX_FRAME 2000
#define MIN_NUM_FEAT 2000

std::string usr_name;
std::string kitti_sequence = "00";

std::string getLoginName()
{
  // get user id
  register struct passwd *pw;
  register uid_t uid;

  uid = geteuid();
  pw = getpwuid(uid);

  if (!pw) {
    std::cerr << "[fatal error] Can't retrieve the username for this UID, "<< (unsigned) uid << std::endl;
    exit(-1);
  }

  std::string login_name(pw->pw_name);    
  
  return login_name;
}

double getAbsoluteScale(int frame_id, int sequence_id, double z_cal)	{
  string line;
  int i = 0;

  std::string gt_filename = "/home/"+usr_name+"/Downloads/data/KITTI/dataset/poses/"+kitti_sequence+".txt";

  ifstream myfile (gt_filename.c_str());
  double x =0, y=0, z = 0;
  double x_prev, y_prev, z_prev;

  if (myfile.is_open()) {
    while (( getline (myfile,line) ) && (i<=frame_id))
    {
      z_prev = z;
      x_prev = x;
      y_prev = y;
      std::istringstream in(line);
      //cout << line << '\n';
      for (int j=0; j<12; j++)  {
        in >> z ;
        if (j==7) y=z;
        if (j==3)  x=z;
      }

      i++;
    }
    myfile.close();
  } else {
    cout << "Unable to open file";
    return 0;
  }

  return sqrt((x-x_prev)*(x-x_prev) + (y-y_prev)*(y-y_prev) + (z-z_prev)*(z-z_prev)) ;
}


int main( int argc, char** argv ){
  Mat img_1, img_2;
  Mat R_f, t_f; //matrices for rotation and translation about the relative camera pose

  // file containing output
  ofstream myfile;
  myfile.open ("results1_1.txt");

  usr_name = getLoginName();

  double scale = 1.00;
  char filename1[200];
  char filename2[200];

  std::string file_fullname1 = "/home/" + usr_name + "/Downloads/data/KITTI/dataset/sequences/"+kitti_sequence+"/image_2/%06d.png";
  std::string file_fullname2 = "/home/" + usr_name + "/Downloads/data/KITTI/dataset/sequences/"+kitti_sequence+"/image_2/%06d.png";

  sprintf(filename1, file_fullname1.c_str(), 0);
  sprintf(filename2, file_fullname2.c_str(), 1);

  char text[100];
  int fontFace = FONT_HERSHEY_PLAIN;
  double fontScale = 1;
  int thickness = 1;
  cv::Point textOrg(10, 50);

  //read the first two frames from the dataset
  Mat img_1_c = imread(filename1);
  Mat img_2_c = imread(filename2);

  if ( !img_1_c.data || !img_2_c.data ) {
    std::cerr<< "[error] Cannot read images!!" << std::endl;
    std::cerr<< "[error] e.g.) KITTI data is expected to be at /home/{your_login_name}/Downloads/KITTI/dataset/sequences/" << std::endl;
    return -1;
  }

  cvtColor(img_1_c, img_1, COLOR_BGR2GRAY);
  cvtColor(img_2_c, img_2, COLOR_BGR2GRAY);

  // feature detection, tracking
  // vectors to store the coordinates of the feature points
  vector<Point2f> points1, points2;
  // detect features from img_1
  featureDetection(img_1, points1);
  vector<uchar> status;

  // track the features detected from img_1 in img_2
  featureTracking(img_1, img_2, points1, points2, status);

  //TODO: add a fucntion to load these values directly from KITTI's calib files

  // WARNING: different sequences ihe KITTI VO dataset have different intrinsic/extrinsic parameters
  // focal length 
  double focal = 718.8560;
  // principal point
  cv::Point2d pp(607.1928, 185.2157);

  // Recover the relative camera pose from the estimated essential matrix
  Mat E, R, t, mask;
  E = findEssentialMat(points2, // Array of N 2d points, the coordinates of these points should be floating-point
		       points1, // Array of N 2d points
		       focal,   // focal length of the camera that is used to acquire images in test
		       pp,      // principal point
		       RANSAC,  // method to estimate a fundamental or essential matrix, RANSAC | MEDS
		       0.999,   // parameter used for RANSAC
		       1.0,     // parameter for RANSAC or LMeds, specifying the desirable level of confidence (or probability) that the estimated matrix is correct.
		       mask);   // Output array of N elements, indicating 0 for outlier and 1 for other.
  
  // recover relative camera pose, rotation and translation, from the estimated essential matrix and the correspont points in two images
  recoverPose(E,       // Input essential matrix
	      points2, // array of N 2d points
	      points1, //
	      R,       // resulting, recovered rotation matrix
	      t,       // resulting, recovered translation matrix
	      focal,   // focal length of the camera
	      pp,      // principal point
	      mask     // Output array of N elements, indicating 0 for outlier and 1 for other.
    );

  Mat prevImage = img_2;
  Mat currImage;
  vector<Point2f> prevFeatures = points2;
  vector<Point2f> currFeatures;

  char filename[100];

  R_f = R.clone();
  t_f = t.clone();

  clock_t begin = clock();

  namedWindow( "Frontal-facing camera", WINDOW_AUTOSIZE );// Create a window for display.
  namedWindow( "Trajectory", WINDOW_AUTOSIZE );// Create a window for display.

  Mat traj = Mat::zeros(600, 600, CV_8UC3);

  clock_t perframe_begin, perframe_end;

  for(int numFrame=2; numFrame < MAX_FRAME; numFrame++)	{
    perframe_begin = clock();
    sprintf(filename, file_fullname1.c_str(), numFrame);

    //cout << numFrame << endl;
    Mat currImage_c = imread(filename);
    cvtColor(currImage_c, currImage, COLOR_BGR2GRAY);
    vector<uchar> status;
    
    featureTracking(prevImage, currImage, prevFeatures, currFeatures, status);
    
    E = findEssentialMat(currFeatures, prevFeatures, focal, pp, RANSAC, 0.999, 1.0, mask);
    recoverPose(E, currFeatures, prevFeatures, R, t, focal, pp, mask);
    
    Mat prevPts(2,prevFeatures.size(), CV_64F), currPts(2,currFeatures.size(), CV_64F);

    // Visualize the optical flow of the features to track
    //this (x,y) combination makes sense as observed from the source code of triangulatePoints on GitHub
    for(int i=0;i<prevFeatures.size();i++)	{   
      prevPts.at<double>(0,i) = prevFeatures.at(i).x;
      prevPts.at<double>(1,i) = prevFeatures.at(i).y;

      currPts.at<double>(0,i) = currFeatures.at(i).x;
      currPts.at<double>(1,i) = currFeatures.at(i).y;

      // drawing the features and their motion vector
      circle(currImage_c, Point(prevFeatures.at(i).x, prevFeatures.at(i).y),
	     0.5, CV_RGB(255,0,0), 2);

      circle(currImage_c, Point(currFeatures.at(i).x, currFeatures.at(i).y),
        0.5, CV_RGB(0,255,0), 2);

      line(currImage_c,
	   Point(prevFeatures.at(i).x, prevFeatures.at(i).y),
	   Point(currFeatures.at(i).x, currFeatures.at(i).y),
	   CV_RGB(0,0,255),
	   1,
	   CV_AA,
	   0
	   );
    }

    scale = getAbsoluteScale(numFrame, 0, t.at<double>(2));

    if ((scale>0.1)&&(t.at<double>(2) > t.at<double>(0)) && (t.at<double>(2) > t.at<double>(1))) {
      t_f = t_f + scale*(R_f*t);
      R_f = R*R_f;

    } else {
     cout << "scale below 0.1, or incorrect translation" << endl;
    }

   // lines for printing results
   // myfile << t_f.at<double>(0) << " " << t_f.at<double>(1) << " " << t_f.at<double>(2) << endl;

    // a redetection is triggered in case the number of feautres being trakced go below a particular threshold
    if (prevFeatures.size() < MIN_NUM_FEAT){
      //cout << "Number of tracked features reduced to " << prevFeatures.size() << endl;
      //cout << "trigerring redection" << endl;
      featureDetection(prevImage, prevFeatures);
      featureTracking(prevImage,currImage,prevFeatures,currFeatures, status);
    }

    prevImage = currImage.clone();
    prevFeatures = currFeatures;

    int x = int(t_f.at<double>(0)) + 300;
    int y = int(t_f.at<double>(2)) + 100;
    circle(traj, Point(x, y), 0.5, CV_RGB(255,0,0), 2);

    rectangle( traj, Point(10, 30), Point(550, 50), CV_RGB(0,0,0), CV_FILLED);
    sprintf(text, "Coordinates: x = %02fm y = %02fm z = %02fm", t_f.at<double>(0), t_f.at<double>(1), t_f.at<double>(2));
    putText(traj, text, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);

    imshow( "Frontal-facing camera", currImage_c );
    imshow( "Trajectory", traj );

    waitKey(1);
    perframe_end = clock();

    cout << "elapsed time to process, " << filename << ", " << (double(perframe_end-perframe_begin)/CLOCKS_PER_SEC) << " sec" << endl;

    //cout << R_f << endl;
    //cout << t_f << endl;
  }

  clock_t end = clock();
  cout << "elapsed time=" << (double(end - begin)/CLOCKS_PER_SEC) << " sec" << endl;

  //cout << R_f << endl;
  //cout << t_f << endl;

  return 0;
}
