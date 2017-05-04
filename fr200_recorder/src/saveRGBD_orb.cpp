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
using namespace sensor_msgs;
using namespace message_filters;



using namespace std;
using namespace cv ; 


ofstream tsfile;

int frame_id = 0 ;  
 
void ImageGrabber(const sensor_msgs::ImageConstPtr& msgRGB,const sensor_msgs::ImageConstPtr& msgD)
{
    // Copy the ros image message to cv::Mat.
    cv_bridge::CvImageConstPtr cv_ptrRGB;
    try
    {
        cv_ptrRGB = cv_bridge::toCvShare(msgRGB, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    cv_bridge::CvImageConstPtr cv_ptrD;
    try
    {
        cv_ptrD = cv_bridge::toCvShare(msgD, sensor_msgs::image_encodings::TYPE_16UC1);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    frame_id++ ;

    string index = std::to_string(frame_id);

    if (frame_id < 10) {
        index = "00000" + index;
    }
    else if (frame_id < 100) {
        index = "0000" + index;
    }
    else if (frame_id < 1000) {
        index = "000" + index;
    }
    else if (frame_id < 10000) {
        index = "00" + index;
    }
    else if (frame_id < 100000) {
        index = "0" + index;
    }
    

    std_msgs::Header h_rgb = msgRGB->header;
    std_msgs::Header h_d = msgRGB->header; 


    cout<< index << "  "<< "rgb_stamp: "<< h_rgb.stamp <<endl; 

    cout << typeid(h_rgb.stamp).name() <<endl;

    imshow("Live Feed RGB: ", cv_ptrRGB->image);  
    waitKey(3); 
    imshow("Live Feed Depth: ", cv_ptrD->image);  
    waitKey(3); 
    stringstream tt;
    tt << h_rgb.stamp ;
    string tt_name = tt.str();

    imwrite("/home/jin/myImage/color/" + tt_name+".png", cv_ptrRGB->image);
    imwrite("/home/jin/myImage/depth/" + tt_name+".png", cv_ptrD->image);

    stringstream ss;
    ss << h_rgb.stamp << "  "<< "color/"<< h_rgb.stamp << ".png  "<<h_d.stamp << " depth/"<< h_rgb.stamp << ".png";
    string time_info = ss.str();

    //ofstream tsfile;
    //tsfile.open("myImage/timestamp.txt");
    tsfile << time_info <<"\n";
    waitKey(3); 
/*    if ((h_rgb.stamp - h_d.stamp)*(h_rgb.stamp - h_d.stamp) < 0.05) {
        t_avg = (h_rgb.stamp + h_d.stamp)/2;
        string timestamp = t_avg.str();
        cout << "Frame: " << index << "; Timestamp: " << timestamp << endl;
        tsfile << index <<" "<<timestamp<<"\n";
    }
    else {
       cout << "Frames Mismatch!" << endl;
    }*/

}

int main(int argc, char **argv)
{
    mkdir("/home/jin/myImage", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("/home/jin/myImage/color", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("/home/jin/myImage/depth", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    tsfile.open("/home/jin/myImage/timestamp.txt");

    ros::init(argc, argv, "SaveRGBD");
    ros::start();

    ros::NodeHandle nh;

    message_filters::Subscriber<sensor_msgs::Image> rgb_sub(nh, "/camera/color/image_raw", 1);
    message_filters::Subscriber<sensor_msgs::Image> depth_sub(nh, "/camera/depth/image_raw", 1);
    
    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, sensor_msgs::Image> sync_pol;
    message_filters::Synchronizer<sync_pol> sync(sync_pol(10), rgb_sub,depth_sub);
    
    //TimeSynchronizer<sensor_msgs::Image, sensor_msgs::Image> sync(rgb_sub, depth_sub, 10);
    
    sync.registerCallback(boost::bind(&ImageGrabber,_1,_2));
    ros::spin();

    ros::shutdown();

    return 0;
}

