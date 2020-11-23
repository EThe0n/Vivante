#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <opencv2/opencv.hpp>

#include "simple_sobel.h"
#include "CLContext.h"
#include "Timer.h"

using namespace cv;

constexpr int KEY_ESCAPE = 27;
constexpr int KEY_0		 = 48;
constexpr int KEY_1		 = 49;
constexpr int KEY_2		 = 50;
constexpr int KEY_3		 = 51;
constexpr int KEY_4		 = 52;
constexpr int KEY_9		 = 57;
constexpr int KEY_MINUS  = 45;

constexpr const char* FILTER_CPU_STR 	= "CPU";
constexpr const char* FILTER_OPENCV_STR = "OpenCV";
constexpr const char* FILTER_OPENCL_STR = "OpenCL";

float gFontSize_ = 1.0f;
bool gIsLooping = false;

enum class FilterContext : int
{
	None = -1, 	// Do not perform edge detection
	Simple_Sobel,
	OpenCV_Sobel,
	OpenCL_Sobel
};

bool readFrame(VideoCapture& videoStream, UMat& frame)
{
	videoStream >> frame;

	if (frame.empty()) {
		return false;
	}

	return true;
}

void opencv_sobel(UMat& frame, Timer& timer)
{
	UMat grad_x, grad_y;
	UMat abs_grad_x, abs_grad_y;
	
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	/// Gradient X
	cv::Sobel(frame, grad_x, CV_16S, 1, 0);
	cv::convertScaleAbs(grad_x, abs_grad_x);

	/// Gradient Y
	cv::Sobel(frame, grad_y, CV_16S, 0, 1);
	cv::convertScaleAbs(grad_y, abs_grad_y);

	// Total Gradient (approximate)
	cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, frame);

	std::chrono::duration<double> elapsedTime_sec = std::chrono::system_clock::now() - start;
	timer.update(elapsedTime_sec.count() * 1000.0);
}

void simple_sobel(UMat& frame, int width, int height, Timer& timer)
{
	cv::Mat src(height, width, CV_8UC1);
	cv::Mat dst(height, width, CV_8UC1);

	frame.copyTo(src);

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	sobel_operator(src.data, dst.data, width, height);

	std::chrono::duration<double> elapsedTime_sec = std::chrono::system_clock::now() - start;
	timer.update(elapsedTime_sec.count() * 1000.0);

	dst.copyTo(frame);
}

void printLog(UMat& frame, const FilterContext& filterContext, Timer timer[])
{
	static FilterContext prevFilter = FilterContext::None;
	const char* filterName[] = { FILTER_CPU_STR, FILTER_OPENCV_STR, FILTER_OPENCL_STR };
	char buffer[128] = "";

	if (prevFilter == FilterContext::None) {
		sprintf(buffer, "[NONE]");
	}
	else {
		int index = (int)prevFilter;
		sprintf(buffer, "[%s] Min: %.2lf, Max: %.2lf, Avg: %2.lf, Now: %.2lf  (ms/frame)", 
			filterName[index], timer[index].minTime_ms, timer[index].maxTime_ms, timer[index].getAverageTime(), timer[index].currentTime_ms);
	}

	int relativeYPos = (int)(30.0 * gFontSize_);
	putText(frame, buffer, Point(10, relativeYPos), cv::FONT_HERSHEY_SIMPLEX, gFontSize_, Scalar(255, 255, 255), 2);

	prevFilter = filterContext;
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

	const double videoFPS = videoStream.get(cv::CAP_PROP_FPS);
	const double refreshTime_ms = 1000.0 / videoFPS;
	int videoWidth_ = (int)videoStream.get(cv::CAP_PROP_FRAME_WIDTH);
	int videoHeight_ = (int)videoStream.get(cv::CAP_PROP_FRAME_HEIGHT);

	printf("Get video information successfully. \n");

	CLContext* clContext = nullptr;
	try {
		clContext = new CLContext(videoWidth_, videoHeight_);
	}
	catch (const std::exception& e) {
		fprintf(stderr, "Error(OpenCL Setup) : %s \n", e.what());
		exit(EXIT_FAILURE);
	}
	printf("OpenCL Context setup finished. \n");

	printf("Press (1/2/3/4) to switch between filters \n");
	printf("1: None, 2:CPU, 3:OpenCV, 4:OpenCL \n");
	printf("Press (9/0) to make font smaller/larger \n");
	printf("Press (-) to loop/unloop video \n");
	UMat frame;
	FilterContext filterContext = FilterContext::None;
	Timer timer[3];
	int frameCounter = 0;
	while (true) {
		if (readFrame(videoStream, frame) == false) {
			if (gIsLooping) {
				frameCounter = 0;
				for (int i = 0; i < 3; ++i) {
					timer[i].reset();
				}
				videoStream.set(CAP_PROP_POS_MSEC, 0.0);
				continue;
			}
			break;
		}

		if (filterContext != FilterContext::None) {
			cvtColor(frame, frame, COLOR_BGR2GRAY);
		}

		// do edge detection
		switch ((int)filterContext) {
		case (int)FilterContext::Simple_Sobel:	
			simple_sobel(frame, videoWidth_, videoHeight_, timer[(int)FilterContext::Simple_Sobel]);
			break;
		case (int)FilterContext::OpenCV_Sobel:	
			opencv_sobel(frame, timer[(int)FilterContext::OpenCV_Sobel]); 
			break;
		case (int)FilterContext::OpenCL_Sobel:	
			clContext->sobel(frame, timer[(int)FilterContext::OpenCL_Sobel]);
			break;
		}
		
		printLog(frame, filterContext, timer);
		cv::imshow("Video player", frame);

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
			gIsLooping = !gIsLooping;
			printf("Video Loop : %s \n", gIsLooping ? "TRUE" : "FALSE");
			break;
		}
	}

	delete clContext;
	destroyAllWindows();

	return 0;
}