This is my modification of Avi Singh's implementation on visual
odometry to reflash what I learned earlier about the relative pose of
a camera. See the following for Avi's visual odometry,
http://avisingh599.github.io/vision/monocular-vo/

#Pipeline 

The visual odometry is a technique for estimating the pose of a rigid
body over time by analyzing the images acquired from the camera
installed or rigidly mounted on the body. This is roughly based on an
assumption that the body is moving relative to other static objects
around it [Howard, 2008], [Fraundorfer and Scaramuzza, 2012], [Nister
et al., 2006], [Scaramuzza and Fraundorfer, 2011].

<ol>

<li><b>Feature Extraction</b>: Extract features from input images to
track over frame. For this, <a
href="https://www.edwardrosten.com/work/rosten_2006_machine.pdf">FAST</a>
[Rosten and Drummond, 2006] is used.

<li><b>Feature Tracking</b>: To estimate the relative motion of a
camera in consecutive images, one needs to track the extracted
features. In other words, assuming that those features are relatively
static, what we want to do is to estimate the motion of the camera or
a body the camera rigidly mounted on with respect to those features
appearing on consecutive image frames. For the feature tracking, the
<a
href="https://en.wikipedia.org/wiki/Kanade%E2%80%93Lucas%E2%80%93Tomasi_feature_tracker">KLT</a>
(Kanade-Lucas-Tomasi) is used.

<li><b>Camera Motion Estimation</b>:The motion of a camera in
consecutive image frames or poses of the body is represented by a
transformation matrix. The transformation matrix is in turn
represented by a composite of a rotation and a translation matrix. The
feature tracking will provide one with a list of the corresponding
features in consecutive image frames. Given this set of the
correspondending features, one can estimate such a transformation
matrix using any of <i>n</i>-point algorithms [Quan and Lan,
1999]. Here the Nister's five point algorithm is used to estimate the
<a href="https://en.wikipedia.org/wiki/Essential_matrix">essential
matrix</a> [Nister, 2004].

<li><b>Computing R,t from the Essential Matrix</b>: Given an estimated
essential matrix, to estimate the relative motion of a camera, one
needs to compute R, t from the essential matrix, E [Hartley and
Zisserman, 2000].

<li><b>Rolling Out the Estimated Trajectory</b>:
</ol>

#Reference
<ul>

<li>[Cheng et al., 2005] Yang Cheng, Mark Maimone, and Larry Matthies,
<a
href="https://www-robotics.jpl.nasa.gov/publications/Mark_Maimone/smc05_cheng_maimone_matthies.pdf">Visual
odometry on the mars exploration rovers</a>, 2005.

<li>[Fraundorfer and Scaramuzza, 2012] Friedrich Fraundorfer and
Davide Scaramuzza, <a
href="https://sites.google.com/site/scarabotix/tutorial-on-visual-odometry">Visual
odometry: part II - matching, robustness, and applications</a>,
<i>IEEE Robotics and Automation Magazine</i>, 19(2): , 2012.

<li>[Hartley and Zisserman, 2000] Richard Hartley and Andrew
Zisserman, <i>Multiview Geometry</i>, 2000.

<li>[Howard, 2008] Andrew Howard, <a href="
https://pdfs.semanticscholar.org/0a6d/a5191a51c097e4b52153a7b426d79b3d634e.pdf">Real-time
stereo visual odometry for autonomous ground vehicles</a>, In
<i>Proceedings of IEEE/RSJ International Conference on Intelligent
Robots and Systems</i> (IROS-2008), pp. , 2008.

<li>[Nister, 2004] David Nister, <a
href="http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.86.8769&rep=rep1&type=pdf">An
efficient solution to the five-point relative pose problem</a>,
<i>IEEE Transactions on Pattern Analysis and Machine Intelligence</i>,
26(6): 756-770, 2004.

<li>[Nister et al., 2004] David Nister, Oleg Naroditsky, and James
Bergen, Visual odometry, In <i>Proceedings of Computer Vision and
Pattern Recognition</i> (CVPR-2004), 2004.

<li>[Nister et al., 2006] David Nister, Oleg Naroditsky, and James
Bergen, <a
href="https://pdfs.semanticscholar.org/c896/5cc5c62a245593dbc679aebdf3338bb945fc.pdf">Visual
odometry for ground vehicle applications</a>, <i>Journal of Field
Robotics</i>, 23(1): 3-20, 2006.

<li>[Rosten and Drummond, 2006] Edward Rosten and Tom Drummond, <a
href="https://www.edwardrosten.com/work/rosten_2006_machine.pdf">Machine
learning for high-speed corner detection</a>, In <i>Proceedings of
European Conference on Computer Vision</i> (ECCV-2006), pp. 430-443,
2006.

<li>[Scaramuzza and Fraundorfer, 2011] Davide Scaramuzza and Friedrich
Fraundorfer, <a
href="https://sites.google.com/site/scarabotix/tutorial-on-visual-odometry">Visual
odometry: part I - the first 30 years and fundamentals</a>, <i>IEEE
Robotics and Automation Magazine</i>, 18(4): , 2011.

<li>[Quan and Lan, 1999] Long Quan and Zhongdan Lan, <a
href="https://hal.archives-ouvertes.fr/inria-00590105/document">Linear
N-point pose determination</a>, <i>IEEE Transactions on Pattern
Analysis and Machine Intelligence</i>, 21(8): 774-780, 1999.

</ul>

##Requirements
OpenCV 3.0

##Build
To build, do the following:
```bash
mkdir build && cd build
cmake ..
make
```

##Run the executable
After compilation, in the build directly, type the following:
```bash
./vo
```
##Before you run
Note that the resulting codes won't provide you with reliable relative
scale estimation, so the scale informaion is extracted from the KITTI
dataset ground truth files.

Thus you need to have either your own data, or else the sequences from
[KITTI's Visual Odometry
Dataset](http://www.cvlibs.net/datasets/kitti/eval_odometry.php). This
implementation assumes that the KITTI data is downloaded at
/home/your_login_id/data/KITTI/ and uses the following for the scale
computation, /home/your_login_id/data/KITTI/dataset/poses/00.txt

In order to run this algorithm on your own data, you must modify the
intrinsic calibration parameters in the code.

##Performance
![Results on the KITTI VO Benchmark](http://avisingh599.github.io/images/visodo/2K.png)

##License
MIT