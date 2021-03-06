/*
 *  Apr. 19 2017, He Zhang, hxzhang1@ualr.edu 
 *
 *  save data from realsense driver 
 *
 * */

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <typeinfo>


#include <chrono>
#include <algorithm>
#include <sys/stat.h>
#include <ros/ros.h>

#include <sensor_msgs/image_encodings.h>  

#include <opencv2/objdetect/objdetect.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/core/core.hpp>

#include <std_msgs/String.h>  

#include <cv_bridge/cv_bridge.h>

#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>

// #include "pcl/visualization/cloud_viewer.h"

#include <mutex>
// #include "glob_def.h"
// #include "pc_from_image.h"
// #include "vtk_viewer.h"

#include <string>
// #include "cam_model.h"


#include <iomanip>
#include <ctime>
#include <sstream>

using namespace sensor_msgs;
using namespace message_filters;

using namespace std;
using namespace cv ; 

ofstream tsfile;

int frame_id = 0 ;  
// cv::Mat gRGB; 
// cv::Mat gDpt; 

string gDir("/home/jin/myImage"); 
string gDataName("OBS");
int gDataNum = -1; 
int gCnt =0; 
bool bStop = false; 

// void ImageGrabber(const sensor_msgs::ImageConstPtr& msgRGB,const sensor_msgs::ImageConstPtr& msgD, const sensor_msgs::ImageConstPtr& msgIr1, const sensor_msgs::ImageConstPtr& msgIr2, const sensor_msgs::CameraInfoConstPtr& irCam)
void ImageGrabber(const sensor_msgs::ImageConstPtr& msgRGB,const sensor_msgs::ImageConstPtr& msgD)
{
    // Copy the ros image message to cv::Mat.
    cv_bridge::CvImageConstPtr cv_ptrRGB;
    cv_bridge::CvImageConstPtr cv_ptrD;

    try
    {
        cv_ptrRGB = cv_bridge::toCvShare(msgRGB, sensor_msgs::image_encodings::BGR8);
        // cv_ptrD = cv_bridge::toCvShare(msgD, sensor_msgs::image_encodings::TYPE_16UC1);
        cv_ptrD = cv_bridge::toCvShare(msgD, sensor_msgs::image_encodings::TYPE_32FC1);
        // cv_ptrIr1 = cv_bridge::toCvShare(msgIr1, sensor_msgs::image_encodings::TYPE_8UC1); 
        // cv_ptrIr2 = cv_bridge::toCvShare(msgIr2, sensor_msgs::image_encodings::TYPE_8UC1); 
        // ROS_WARN("Receive msg rgb w %d h %d, dpt w %d h %d ir w %d h %d!", cv_ptrRGB->image.cols, cv_ptrRGB->image.rows, cv_ptrD->image.cols, cv_ptrD->image.rows, cv_ptrIr1->image.cols, cv_ptrIr1->image.rows); 
	ROS_WARN("received data!"); 
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }
  

    stringstream tt;
    tt << msgRGB->header.stamp ;
    string tt_name = tt.str();

    imshow("rgb ", cv_ptrRGB->image); 
    cv::waitKey(3);

    imwrite( gDir + "/" + gDataName + "/color/" + tt_name+".png", cv_ptrRGB->image);
    //imwrite("/home/jin/myImage/tmp.png", cv_ptrRGB->image);
	  // imwrite( gDir + "/" + gDataName + "/depth/" + tt_name+".png", cv_ptrD->image);
    // as suggested to store CV_32FC1 in .exr 
    imwrite( gDir + "/" + gDataName + "/depth/" + tt_name+".exr", cv_ptrD->image);
    
    //imwrite( gDir + "/" + gDataName + "/ir/" + tt_name+".png", cv_ptrIr1->image);
    //imwrite( gDir + "/" + gDataName + "/ir2/" + tt_name+".png", cv_ptrIr2->image);
    ROS_WARN("wrote data!"); 

    std_msgs::Header h_rgb = msgRGB->header;
    std_msgs::Header h_d = msgRGB->header; 
    // std_msgs::Header h_ir = msgIr1->header; 
    // std_msgs::Header h_ir2 = msgIr2->header; 

    stringstream ss;
    // ss << h_rgb.stamp << "  "<< "color/"<< h_rgb.stamp << ".png  "<<h_d.stamp << " depth/"<< h_rgb.stamp << ".exr "<< h_ir.stamp<<" ir/"<<h_ir.stamp<<".png "<<h_ir2.stamp<<" ir2/"<<h_ir2.stamp<<".png";
    ss << h_rgb.stamp << "  "<< "color/"<< h_rgb.stamp << ".png  "<<h_d.stamp << " depth/"<< h_rgb.stamp << ".exr "; 
    string time_info = ss.str();
    tsfile << time_info <<"\n";

    if(gDataNum > 0 )
    {
      if(++gCnt > gDataNum)
      {
        bStop = true; // save enough images 
      }
    }
    return; 
}

int main(int argc, char* argv[])
{

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << "OBS_"<<oss;
    auto str = oss.str();

    std::cout << str << std::endl;

  ros::init(argc, argv, "realsense_wrapper"); 
  ros::start(); 
  ros::NodeHandle nh; 

  ros::NodeHandle np("~"); 
  np.param("data_dir", gDir, gDir); 
  np.param("data_name", gDataName, gDataName); 
  np.param("data_num", gDataNum, gDataNum);

  string mainDir = "/home/jin/myImage";
  mkdir(mainDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  string d_dir = gDir +"/"+ str;//gDataName; 
  gDataName = str;
  string d_rgb = d_dir + "/color"; 
  string d_dpt = d_dir + "/depth"; 
  string d_ir = d_dir + "/ir"; 
  string d_ir2 = d_dir + "/ir2"; 
  std::cout<<d_dir<<"   d_dir_got|||"<<d_rgb<<"   d_rgb|||"<<d_dpt<<"   d_dpt|||"<<std::endl;

  

  mkdir(d_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  mkdir(d_rgb.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  mkdir(d_dpt.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  // mkdir(d_ir.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  // mkdir(d_ir2.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  string f_time = d_dir + "/timestamp.txt"; 
  tsfile.open(f_time.c_str());
  tsfile << "index  timestamp\n";

  
  int q = 7; 
  message_filters::Subscriber<sensor_msgs::Image> rgb_sub(nh, "/camera/rgb/image_rect_color", q);
  message_filters::Subscriber<sensor_msgs::Image> depth_sub(nh, "/camera/depth_registered/sw_registered/image_rect", q);

 typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, sensor_msgs::Image> sync_pol;
    message_filters::Synchronizer<sync_pol> sync(sync_pol(q), rgb_sub, depth_sub);
    //TimeSynchronizer<sensor_msgs::Image, sensor_msgs::Image> sync(rgb_sub, depth_sub, 10);

    // sync.registerCallback(boost::bind(&ImageGrabber,_1,_2, _3, _4, _5));
    sync.registerCallback(boost::bind(&ImageGrabber, _1, _2)); 
    ROS_WARN("realsense_wrapper.cpp: start to subscribe msgs!"); 

    ros::spin(); 
 
    ros::shutdown(); 

    return 0;
}








