#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include "simple_sobel.h"

using namespace cv;

constexpr int KEY_ESCAPE = 27;
constexpr int KEY_0		 = 48;
constexpr int KEY_1		 = 49;
constexpr int KEY_2		 = 50;
constexpr int KEY_3		 = 51;
constexpr int KEY_4		 = 52;
constexpr int KEY_9		 = 57;
constexpr int KEY_MINUS  = 45;

constexpr const char* FILTER_OPENCL_STR = "OpenCL";
constexpr const char* FILTER_OPENCV_STR = "OpenCV";
constexpr const char* FILTER_CPU_STR	= "CPU";

float gFontSize_ = 1.0f;
bool gIsVideoLoop = false;

enum class FilterContext : int
{
	None, 	// Do not perform edge detection
	Simple_Sobel,
	OpenCV_Sobel,
	OpenCL_Sobel
};

bool readFrame(VideoCapture& videoStream, UMat& frame_BGR)
{
	videoStream >> frame_BGR;

	if (frame_BGR.empty()) {
		return false;
	}

	return true;
}

void opencl_sobel(UMat& frame)
{
	
}

void opencv_sobel(UMat& frame)
{
	UMat grad_x, grad_y;
	UMat abs_grad_x, abs_grad_y;
	
	/// Gradient X
	cv::Sobel(frame, grad_x, CV_16S, 1, 0);
	cv::convertScaleAbs(grad_x, abs_grad_x);

	/// Gradient Y
	cv::Sobel(frame, grad_y, CV_16S, 0, 1);
	cv::convertScaleAbs(grad_y, abs_grad_y);

	// Total Gradient (approximate)
	cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, frame);
}

void simple_sobel(UMat& frame, int width, int height)
{
	cv::Mat src(height, width, CV_8UC1);
	cv::Mat dst(height, width, CV_8UC1);

	frame.copyTo(src);

	sobel_operator(src.data, dst.data, width, height);

	dst.copyTo(frame);
}

void printLog(UMat& frame)
{
	int relativeYPos = (int)(30.0 * gFontSize_);
	putText(frame, "Hello world!", Point(10, relativeYPos), cv::FONT_HERSHEY_SIMPLEX, gFontSize_, Scalar(255, 255, 255), 2);
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage : ./player video \n");
		exit(EXIT_FAILURE);
	}

	VideoCapture videoStream(argv[1]);
	if (!videoStream.isOpened()) {
		fprintf(stderr, "Failed to open file %s \n", argv[1]);
		exit(EXIT_FAILURE);
	}

	printf("Press (1/2/3/4) to switch between filters \n");
	printf("1: None, 2:CPU, 3:OpenCV, 4:OpenCL \n");

	const double videoFPS = videoStream.get(cv::CAP_PROP_FPS);
	const double refreshTime_ms = 1000.0 / videoFPS;
	int videoWidth_ = (int)videoStream.get(cv::CAP_PROP_FRAME_WIDTH);
	int videoHeight_ = (int)videoStream.get(cv::CAP_PROP_FRAME_HEIGHT);

	UMat frame;
	FilterContext filterContext = FilterContext::None;
	while (true) {
		if (readFrame(videoStream, frame) == false) {
			if (gIsVideoLoop) {
				videoStream.set(CAP_PROP_POS_MSEC, 0.0);
				continue;
			}
			break;
		}

		cvtColor(frame, frame, COLOR_BGR2GRAY);

		// do edge detection
		switch ((int)filterContext) {
		case (int)FilterContext::Simple_Sobel:	simple_sobel(frame, videoWidth_, videoHeight_); break;
		case (int)FilterContext::OpenCV_Sobel:	opencv_sobel(frame); break;
		case (int)FilterContext::OpenCL_Sobel:	opencl_sobel(frame); break;
		}

		printLog(frame);

		imshow("Video player", frame);

		int keyCode = waitKey(refreshTime_ms);
		if (keyCode == KEY_ESCAPE) {
			break;
		}

		// key event
		switch (keyCode) {
		case KEY_1:	filterContext = FilterContext::None;			 break;
		case KEY_2: filterContext = FilterContext::Simple_Sobel;	 break;
		case KEY_3: filterContext = FilterContext::OpenCV_Sobel;	 break;
		case KEY_4: filterContext = FilterContext::OpenCL_Sobel;	 break;

		case KEY_0: gFontSize_ = std::min(gFontSize_ + 0.25f, 5.0f); break;
		case KEY_9: gFontSize_ = std::max(0.0f, gFontSize_ - 0.25f);  break;

		case KEY_MINUS: 
			gIsVideoLoop = !gIsVideoLoop;		
			printf("Video Loop : %s \n", gIsVideoLoop ? "TRUE" : "FALSE");
			break;
		}
	}

	destroyAllWindows();

	return 0;
}