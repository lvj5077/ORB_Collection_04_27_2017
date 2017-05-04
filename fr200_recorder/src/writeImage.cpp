#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include <string>

using namespace std;
using namespace cv ; 



int main(int argc, char* argv[])
{
	Mat image;
    image = imread("/home/jin/Desktop/rviz.png",0); 
    // imshow( "Display window", image ); 

	imwrite( "/home/jin/Desktop/wrote_rviz.png", image);

	return 0;
}




