This is my modification of Avi Singh's implementation on visual
odometry. See the following for Avi's visual odometry,
http://avisingh599.github.io/vision/monocular-vo/

#Pipeline
<ol>
<li><b>Feature Extraction</b>: Extract features from input images to track over frame. For this, <a href="https://www.edwardrosten.com/work/rosten_2006_machine.pdf">FAST</a> is used. 
```bash
[Rosten and Drummond, 2006] Edward Rosten and Tom Drummond, Machine learning for high-speed corner detection, In <i>Proceedings of European Conference on Computer Vision</i> (ECCV-2006), pp. 430-443, 2006
```
<li><b>Feature Tracking</b>: To estimate the relative motion of a camera in consecutive images, one needs to track the extracted features. In other words, assuming that those features are relatively static, what we want to do is to estimate the motion of the camera or a body the camera rigidly mounted on with respect to those features. For the feature tracking, KLT (Kanade-Lucas-Tomasi) is used.
<li><b>Camera Motion Estimation</b>:The motion of a camera in consecutive image frames or poses of the body is represented by a transformation matrix. The transformation matrix is in turn represented by a composite of a rotation and a translation matrix. Given a correspondence or a set of the correspondent features, one can estimate such a transformation matrix using any of <i>n</i>-point algorithms. Here the Nister's five point algorithm is used to estimate the essential matrix. 
<li><b>Computing R,t from the Essential Matrix</b>:
<li><b>Rolling Out the Estimated Trajectory</b>:
</ol>

Note that this project is not yet capable of doing reliable relative
scale estimation, so the scale informaion is extracted from the KITTI
dataset ground truth files.

##Requirements
OpenCV 3.0

##Build
Provided with this repo is a CMakeLists.txt file, which you can use to directly compile the code as follows:
```bash
mkdir build
cd build
cmake ..
make
```

##How to run? 
After compilation, in the build directly, type the following:
```bash
./vo
```
##Before you run
In order to run this algorithm, you need to have either your own data, 
or else the sequences from [KITTI's Visual Odometry Dataset](http://www.cvlibs.net/datasets/kitti/eval_odometry.php).
In order to run this algorithm on your own data, you must modify the intrinsic calibration parameters in the code.

##Performance
![Results on the KITTI VO Benchmark](http://avisingh599.github.io/images/visodo/2K.png)

##License
MIT