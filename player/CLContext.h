#pragma once

#include "cl_wrapping.h"
#include "Timer.h"

#include <opencv2/opencv.hpp>

#include <exception>
#include <string>
#include <fstream>
#include <sstream>

class CLContext
{
public:
	CLContext() = delete;
	CLContext(int imgWidth, int imgHeight) :
		imgWidth_(imgWidth),
		imgHeight_(imgHeight)
	{
		cl_platform_id platform = 0;
		try {
			platform = findPlatform();
			device_ = findDevice(platform);
			context_ = cl::createContext(nullptr, 1, &device_, nullptr, nullptr);
			//cl_command_queue_properties commandQueueProperties[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
			//commandQueue_ = cl::createCommandQueueWithProperties(context_, device_, commandQueueProperties);
			commandQueue_ = clCreateCommandQueue(context_, device_, CL_QUEUE_PROFILING_ENABLE, NULL);

			sobelProgram_ = initProgram("Sobel.cl");

			initImageBuffer();
			initKernel();
			cl::getKernelWorkGroupInfo(sobelKernel_, device_, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size_t), &preferredWorkgroupSize);
		}
		catch (const std::exception& e) {
			throw std::runtime_error(e.what());
		}
	}

	~CLContext()
	{
		cl::releaseContext(context_);
		cl::releaseDevice(device_);
		cl::releaseCommandQueue(commandQueue_);
		cl::releaseProgram(sobelProgram_);
		cl::releaseKernel(sobelKernel_);
		cl::releaseMemObject(inputImage_);
		cl::releaseMemObject(outputImage_);
	}

	void sobel(cv::UMat& frame, Timer& timer)
	{
		size_t workSizeX = (((size_t)imgWidth_ - 1) / preferredWorkgroupSize + 1) * preferredWorkgroupSize;
		size_t globalWorkSize[] = { workSizeX, (size_t)imgHeight_ };
		size_t localWorkSize[] = { preferredWorkgroupSize, 1 };

		size_t origin[] = { 0, 0, 0 };
		size_t region[] = { (size_t)imgWidth_, (size_t)imgHeight_, 1 };

		cl_event writeImage, sobel, readImage;

		try {
			cv::Mat src;
			frame.copyTo(src);

			cl::enqueueWriteImage(commandQueue_, inputImage_, CL_TRUE, origin, region, 0, 0, src.data, 0, nullptr, &writeImage);
			cl::enqueueNDRangeKernel(commandQueue_, sobelKernel_, 2, nullptr, globalWorkSize, localWorkSize, 1, &writeImage, &sobel);
			cl::enqueueReadImage(commandQueue_, outputImage_, CL_TRUE, origin, region, 0, 0, src.data, 1, &sobel, &readImage);
			cl::waitForEvents(1, &readImage);

			timer.update(profile(sobel));

			src.copyTo(frame);
		}
		catch (const std::exception& e) {
			throw std::runtime_error(e.what());
		}
	}

private:
	cl_platform_id findPlatform()
	{
		cl_uint numPlatforms = 0;
		cl_platform_id platform;

		try {
			cl::getPlatformIDs(0, nullptr, &numPlatforms);
			if (numPlatforms < 1) {
				throw std::runtime_error("There is no OpenCL platform.");
			}
			cl::getPlatformIDs(1, &platform, nullptr);
		}
		catch (const std::exception& e) {
			throw std::runtime_error(e.what());
		}

		return platform;
	}

	cl_device_id findDevice(const cl_platform_id& platform)
	{
		cl_uint numDevices = 0;
		cl_device_id device;

		try {
			cl::getDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &numDevices);
			if (numDevices < 1) {
				throw std::runtime_error("There is no OpenCL device.");
			}
			cl::getDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);
		}
		catch (const std::exception& e) {
			throw std::runtime_error(e.what());
		}

		return device;
	}

	std::string readFile(const std::string& fileName)
	{
		std::ifstream srcFile(fileName.c_str(), (std::fstream::in | std::fstream::binary | std::fstream::ate));

		if (srcFile.fail()) {
			std::stringstream ss;
			ss << "Can not open " << fileName;

			throw std::runtime_error(ss.str().c_str());
		}

		size_t fileSize = srcFile.tellg();
		srcFile.seekg(0, std::fstream::beg);

		std::string src;
		src.resize(fileSize + 1);

		srcFile.read(&src[0], fileSize);
		src[fileSize] = '\0';

		return src;
	}

	cl_program initProgram(const std::string& filePath)
	{
		cl_program program = 0;
		try {
			std::string src = readFile(filePath);
			program = cl::createProgramWithSingleSource(context_, src);
			cl::buildProgram(program, 1, &device_, nullptr, nullptr, nullptr);
		}
		catch (const std::exception& e) {
			throw std::runtime_error(e.what());
		}

		return program;
	}

	void initKernel()
	{
		try {
			sobelKernel_ = cl::createKernel(sobelProgram_, "sobel");
			cl::setKernelArg(sobelKernel_, 0, sizeof(cl_mem), &inputImage_);
			cl::setKernelArg(sobelKernel_, 1, sizeof(cl_mem), &outputImage_);
		}
		catch (const std::exception& e) {
			throw std::runtime_error(e.what());
		}
	}

	void initImageBuffer()
	{
		cl_image_format format;
		format.image_channel_order = CL_R;
		format.image_channel_data_type = CL_UNSIGNED_INT8;

		cl_image_desc image_desc;
		image_desc.image_type = CL_MEM_OBJECT_IMAGE2D;
		image_desc.image_width = imgWidth_;
		image_desc.image_height = imgHeight_;
		image_desc.image_array_size = 1;
		image_desc.image_row_pitch = 0;
		image_desc.image_slice_pitch = 0;
		image_desc.num_mip_levels = 0;
		image_desc.num_samples = 0;
		image_desc.buffer = NULL;

		try {
			inputImage_ = cl::createImage(context_, CL_MEM_READ_ONLY, &format, &image_desc, nullptr);
			outputImage_ = cl::createImage(context_, CL_MEM_WRITE_ONLY, &format, &image_desc, nullptr);
		}
		catch (const std::exception& e) {
			throw std::runtime_error(e.what());
		}
	}

	double profile(cl_event& ev)
	{
		cl_ulong startTime = 0;
		cl_ulong endTime = 0;

		clGetEventProfilingInfo(ev, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &startTime, nullptr);
		clGetEventProfilingInfo(ev, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &endTime, nullptr);

		return (double)(endTime - startTime) * 1.0e-6;
	}

private:
	cl_context context_;
	cl_device_id device_;
	cl_command_queue commandQueue_;
	cl_program sobelProgram_;
	cl_kernel sobelKernel_;
	cl_mem inputImage_;
	cl_mem outputImage_;

	size_t preferredWorkgroupSize;
	int imgWidth_;
	int imgHeight_;
};