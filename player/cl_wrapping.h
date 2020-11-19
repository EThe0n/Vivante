#pragma once

#define CL_TARGET_OPENCL_VERSION 120

#include <CL/cl.h>
#include <stdexcept>
#include <sstream>
#include <string>
#include <sstream>

namespace cl
{
	// Convert macro to string
#define CASE_RETURN_STRING(x) case x : return #x;

#ifndef NDEBUG
	// 디버그 시에만 버그가 출력되고 릴리즈 모드일 때는 버그가 출력되지 않도록 한다.
	#define THROW_ERROR_EXCEPTION(x) if (x != CL_SUCCESS) {\
									std::string errLog = getErrorLog(__FUNCTION__, errCode);\
									throw std::runtime_error(errLog.c_str());}
#else 
#define THROW_ERROR_EXCEPTION(x) if (x != CL_SUCCESS) {throw std::runtime_error("");}
#endif

	std::string errorCodeToString(cl_int errCode)
	{
		switch (errCode) {
		CASE_RETURN_STRING(CL_DEVICE_NOT_FOUND)
		CASE_RETURN_STRING(CL_DEVICE_NOT_AVAILABLE)
		CASE_RETURN_STRING(CL_COMPILER_NOT_AVAILABLE)
		CASE_RETURN_STRING(CL_MEM_OBJECT_ALLOCATION_FAILURE)
		CASE_RETURN_STRING(CL_OUT_OF_RESOURCES)
		CASE_RETURN_STRING(CL_OUT_OF_HOST_MEMORY)
		CASE_RETURN_STRING(CL_PROFILING_INFO_NOT_AVAILABLE)
		CASE_RETURN_STRING(CL_MEM_COPY_OVERLAP)
		CASE_RETURN_STRING(CL_IMAGE_FORMAT_MISMATCH)
		CASE_RETURN_STRING(CL_IMAGE_FORMAT_NOT_SUPPORTED)
		CASE_RETURN_STRING(CL_BUILD_PROGRAM_FAILURE)
		CASE_RETURN_STRING(CL_MAP_FAILURE)
		CASE_RETURN_STRING(CL_MISALIGNED_SUB_BUFFER_OFFSET)
		CASE_RETURN_STRING(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST)
		CASE_RETURN_STRING(CL_COMPILE_PROGRAM_FAILURE)
		CASE_RETURN_STRING(CL_LINKER_NOT_AVAILABLE)
		CASE_RETURN_STRING(CL_LINK_PROGRAM_FAILURE)
		CASE_RETURN_STRING(CL_DEVICE_PARTITION_FAILED)
		CASE_RETURN_STRING(CL_KERNEL_ARG_INFO_NOT_AVAILABLE)
		CASE_RETURN_STRING(CL_INVALID_VALUE)
		CASE_RETURN_STRING(CL_INVALID_DEVICE_TYPE)
		CASE_RETURN_STRING(CL_INVALID_PLATFORM)
		CASE_RETURN_STRING(CL_INVALID_DEVICE)
		CASE_RETURN_STRING(CL_INVALID_CONTEXT)
		CASE_RETURN_STRING(CL_INVALID_QUEUE_PROPERTIES)
		CASE_RETURN_STRING(CL_INVALID_COMMAND_QUEUE)
		CASE_RETURN_STRING(CL_INVALID_HOST_PTR)
		CASE_RETURN_STRING(CL_INVALID_MEM_OBJECT)
		CASE_RETURN_STRING(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)
		CASE_RETURN_STRING(CL_INVALID_IMAGE_SIZE)
		CASE_RETURN_STRING(CL_INVALID_SAMPLER)
		CASE_RETURN_STRING(CL_INVALID_BINARY)
		CASE_RETURN_STRING(CL_INVALID_BUILD_OPTIONS)
		CASE_RETURN_STRING(CL_INVALID_PROGRAM)
		CASE_RETURN_STRING(CL_INVALID_PROGRAM_EXECUTABLE)
		CASE_RETURN_STRING(CL_INVALID_KERNEL_NAME)
		CASE_RETURN_STRING(CL_INVALID_KERNEL_DEFINITION)
		CASE_RETURN_STRING(CL_INVALID_KERNEL)
		CASE_RETURN_STRING(CL_INVALID_ARG_INDEX)
		CASE_RETURN_STRING(CL_INVALID_ARG_VALUE)
		CASE_RETURN_STRING(CL_INVALID_ARG_SIZE)
		CASE_RETURN_STRING(CL_INVALID_KERNEL_ARGS)
		CASE_RETURN_STRING(CL_INVALID_WORK_DIMENSION)
		CASE_RETURN_STRING(CL_INVALID_WORK_GROUP_SIZE)
		CASE_RETURN_STRING(CL_INVALID_WORK_ITEM_SIZE)
		CASE_RETURN_STRING(CL_INVALID_GLOBAL_OFFSET)
		CASE_RETURN_STRING(CL_INVALID_EVENT_WAIT_LIST)
		CASE_RETURN_STRING(CL_INVALID_EVENT)
		CASE_RETURN_STRING(CL_INVALID_OPERATION)
		CASE_RETURN_STRING(CL_INVALID_GL_OBJECT)
		CASE_RETURN_STRING(CL_INVALID_BUFFER_SIZE)
		CASE_RETURN_STRING(CL_INVALID_MIP_LEVEL)
		CASE_RETURN_STRING(CL_INVALID_GLOBAL_WORK_SIZE)
		CASE_RETURN_STRING(CL_INVALID_PROPERTY)
		CASE_RETURN_STRING(CL_INVALID_IMAGE_DESCRIPTOR)
		CASE_RETURN_STRING(CL_INVALID_COMPILER_OPTIONS)
		CASE_RETURN_STRING(CL_INVALID_LINKER_OPTIONS)
		CASE_RETURN_STRING(CL_INVALID_DEVICE_PARTITION_COUNT)
		}

		return "";
	}

	std::string getErrorLog(const char* methodName, cl_int errCode) 
	{
		std::stringstream logStream;
		logStream << methodName << ": " << errorCodeToString(errCode);

		return logStream.str();
	}

	void getPlatformIDs(cl_uint num_entries, cl_platform_id* platforms, cl_uint* num_platforms)
	{
		cl_int errCode = clGetPlatformIDs(num_entries, platforms, num_platforms);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void getDeviceIDs(cl_platform_id platform, cl_device_type device_type, cl_uint num_entries, cl_device_id* devices, cl_uint* num_devices)
	{
		cl_int errCode = clGetDeviceIDs(platform, device_type, num_entries, devices, num_devices);
		THROW_ERROR_EXCEPTION(errCode)
	}

	cl_context createContext(
		const cl_context_properties* properties, 
		cl_uint num_devices, 
		const cl_device_id* devices,
		void (CL_CALLBACK* pfn_notify)(const char* errinfo, const void* private_info, size_t cb, void* user_data), 
		void* user_data)
	{
		cl_int errCode = CL_SUCCESS;
		cl_context context = clCreateContext(properties, num_devices, devices, pfn_notify, user_data, &errCode);
		THROW_ERROR_EXCEPTION(errCode)

		return context;
	}

	// cl_command_queue createCommandQueueWithProperties(cl_context context, cl_device_id device, const cl_queue_properties* properties)
	// {
	// 	cl_int errCode = CL_SUCCESS;
	// 	cl_command_queue commandQueue = clCreateCommandQueueWithProperties(context, device, properties, &errCode);
	// 	THROW_ERROR_EXCEPTION(errCode)

	// 	return commandQueue;
	// }

	cl_program createProgramWithSource(cl_context context, cl_uint count, const char** strings, const size_t* lengths)
	{
		cl_int errCode = CL_SUCCESS;
		cl_program program = clCreateProgramWithSource(context, count, strings, lengths, &errCode);
		THROW_ERROR_EXCEPTION(errCode)

		return program;
	}

	cl_program createProgramWithSingleSource(cl_context context, const std::string& str)
	{
		cl_program program;

		try {
			const char* source = str.c_str();
			size_t sourceLen[] = { strlen(source) };
			program = createProgramWithSource(context, 1, &source, sourceLen);
		}
		catch (const std::exception& ex) {
			throw std::runtime_error(ex.what());
		}

		return program;
	}

	cl_mem createBuffer(cl_context context, cl_mem_flags flags, size_t size, void* host_ptr)
	{
		cl_int errCode = CL_SUCCESS;
		cl_mem buffer = clCreateBuffer(context, flags, size, host_ptr, &errCode);
		THROW_ERROR_EXCEPTION(errCode)

		return buffer;
	}

	cl_kernel createKernel(cl_program program, const char* kernel_name)
	{
		cl_int errCode = CL_SUCCESS;
		cl_kernel kernel = clCreateKernel(program, kernel_name, &errCode);
		THROW_ERROR_EXCEPTION(errCode)

		return kernel;
	}
	
	cl_mem createImage(cl_context context, cl_mem_flags flags, const cl_image_format* image_format, const cl_image_desc* image_desc, void* host_ptr = nullptr)
	{
		cl_int errCode = CL_SUCCESS;
		cl_mem memObj = clCreateImage(context, flags, image_format, image_desc, host_ptr, &errCode);
		THROW_ERROR_EXCEPTION(errCode)

		return memObj;
	}

	void setKernelArg(cl_kernel kernel, cl_uint arg_index, size_t arg_size, const void* arg_value)
	{
		cl_int errCode = clSetKernelArg(kernel, arg_index, arg_size, arg_value);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void buildProgram(
		cl_program program, 
		cl_uint num_devices, 
		const cl_device_id* device_list, 
		const char* options, 
		void (CL_CALLBACK* pfn_notify)(cl_program program, void* user_data), 
		void* user_data)
	{
		cl_int errCode = CL_SUCCESS;
		std::stringstream ss;

		errCode = clBuildProgram(program, num_devices, device_list, options, pfn_notify, user_data);
		switch (errCode) {
		case CL_BUILD_PROGRAM_FAILURE:	
			for (cl_uint i = 0; i < num_devices; ++i) {
				std::string log;
				size_t logLength;
				
				clGetProgramBuildInfo(program, device_list[i], CL_PROGRAM_BUILD_LOG, 0, nullptr, &logLength);
				log.resize(logLength);
				clGetProgramBuildInfo(program, device_list[i], CL_PROGRAM_BUILD_LOG, logLength, &log[0], nullptr);

				ss << "Device " << i << '\n' << log << "\n\n";
			}
			throw std::runtime_error(ss.str().c_str());
			break;
		default:
			THROW_ERROR_EXCEPTION(errCode)
		}
		
	}

	void enqueueNDRangeKernel(
		cl_command_queue command_queue, 
		cl_kernel kernel, 
		cl_uint work_dim, 
		const size_t* global_work_offset, 
		const size_t* global_work_size, 
		const size_t* local_work_size, 
		cl_uint num_events_in_wait_list = 0, 
		const cl_event* event_wait_list = nullptr, 
		cl_event* event = nullptr)
	{
		cl_int errCode = clEnqueueNDRangeKernel(command_queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size, num_events_in_wait_list, event_wait_list, event);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void enqueueReadBuffer(
		cl_command_queue command_queue,
		cl_mem buffer,
		cl_bool blocking_read,
		size_t offset,
		size_t size,
		void* ptr,
		cl_uint num_events_in_wait_list = 0,
		const cl_event* event_wait_list = nullptr,
		cl_event* event = nullptr)
	{
		cl_int errCode = clEnqueueReadBuffer(command_queue, buffer, blocking_read, offset, size, ptr, num_events_in_wait_list, event_wait_list, event);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void enqueueWriteBuffer(
		cl_command_queue command_queue,
		cl_mem buffer,
		cl_bool blocking_write,
		size_t offset,
		size_t size,
		const void* ptr,
		cl_uint num_events_in_wait_list = 0,
		const cl_event* event_wait_list = nullptr,
		cl_event* event = nullptr)
	{
		cl_int errCode = clEnqueueWriteBuffer(command_queue, buffer, blocking_write, offset, size, ptr, num_events_in_wait_list, event_wait_list, event);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void enqueueFillImage(
		cl_command_queue command_queue,
		cl_mem image,
		const void* fill_color,
		const size_t* origin,
		const size_t* region,
		cl_uint num_events_in_wait_list = 0,
		const cl_event* event_wait_list = nullptr,
		cl_event* event = nullptr)
	{
		cl_int errCode = clEnqueueFillImage(command_queue, image, fill_color, origin, region, num_events_in_wait_list, event_wait_list, event);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void enqueueWriteImage(
		cl_command_queue command_queue,
		cl_mem image,
		cl_bool blocking_write,
		const size_t* origin,
		const size_t* region,
		size_t input_row_pitch,
		size_t input_slice_pitch,
		const void* ptr,
		cl_uint num_events_in_wait_list = 0,
		const cl_event* event_wait_list = nullptr,
		cl_event* event = nullptr)
	{
		cl_int errCode = clEnqueueWriteImage(command_queue, image, blocking_write, origin, region, input_row_pitch, input_slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void enqueueReadImage(
		cl_command_queue command_queue,
		cl_mem image,
		cl_bool blocking_read,
		const size_t* origin,
		const size_t* region,
		size_t row_pitch,
		size_t slice_pitch,
		void* ptr,
		cl_uint num_events_in_wait_list = 0,
		const cl_event* event_wait_list = nullptr,
		cl_event* event = nullptr)
	{
		cl_int errCode = clEnqueueReadImage(command_queue, image, blocking_read, origin, region, row_pitch, slice_pitch, ptr, num_events_in_wait_list, event_wait_list, event);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void getDeviceInfo(
		cl_device_id device, 
		cl_device_info param_name,
		size_t param_value_size,
		void* param_value,
		size_t* param_value_size_ret = nullptr)
	{
		cl_int errCode = clGetDeviceInfo(device, param_name, param_value_size, param_value, param_value_size_ret);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void getKernelWorkGroupInfo(
		cl_kernel kernel,
		cl_device_id device,
		cl_kernel_work_group_info param_name,
		size_t param_value_size,
		void* param_value,
		size_t* param_value_size_ret = nullptr)
	{
		cl_int errCode = clGetKernelWorkGroupInfo(kernel, device, param_name, param_value_size, param_value, param_value_size_ret);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void waitForEvents(cl_uint num_events, const cl_event* event_list)
	{
		cl_int errCode = clWaitForEvents(num_events, event_list);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void getEventProfilingInfo(
		cl_event event,
		cl_profiling_info param_name,
		size_t param_value_size,
		void* param_value,
		size_t* param_value_size_ret = nullptr)
	{
		cl_int errCode = clGetEventProfilingInfo(event, param_name, param_value_size, param_value, param_value_size_ret);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void releaseEvent(cl_event event)
	{
		cl_int errCode = clReleaseEvent(event);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void releaseDevice(cl_device_id device) 
	{
		cl_int errCode = clReleaseDevice(device);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void releaseMemObject(cl_mem memobj)
	{
		cl_int errCode = clReleaseMemObject(memobj);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void releaseKernel(cl_kernel kernel)
	{
		cl_int errCode = clReleaseKernel(kernel);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void releaseProgram(cl_program program)
	{
		cl_int errCode = clReleaseProgram(program);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void releaseCommandQueue(cl_command_queue command_queue)
	{
		cl_int errCode = clReleaseCommandQueue(command_queue);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void releaseContext(cl_context context)
	{
		cl_int errCode = clReleaseContext(context);
		THROW_ERROR_EXCEPTION(errCode)
	}

	void finish(cl_command_queue command_queue)
	{
		cl_int errCode = clFinish(command_queue);
		THROW_ERROR_EXCEPTION(errCode)
	}
};
