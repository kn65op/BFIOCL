#include "OpenCLAlgorithm.h"

#include <iostream>

#define ASSERT_OPENCL_ERR(ERR,MSG) if(ERR != CL_SUCCESS) \
{ \
  throw OpenCLAlgorithmException(MSG, ERR); \
}

OpenCLAlgorithm::OpenCLAlgorithm(void)
{
  kernel = NULL;
  program = NULL;
}

OpenCLAlgorithm::OpenCLAlgorithm(const OpenCLDevice & dev)
{
  if (!(dev.isValid()))
  {
    throw OpenCLAlgorithmException("Invalid Deice");
  }
  kernel = NULL;
  program = NULL;
  device = dev;
  setDevice();
}

OpenCLAlgorithm::~OpenCLAlgorithm(void)
{
  if (program)  clReleaseProgram(program); 
  if (kernel) clReleaseKernel(kernel); 
}

void OpenCLAlgorithm::setDevice(const OpenCLDevice & dev)
{
  if (dev.isValid())
  {
    device = dev;
    setDevice();
    return;
  }
  throw OpenCLAlgorithmException("Invalid Device");
}

void OpenCLAlgorithm::enqueueNDRangeKernelWithTimeMeasurment(cl_uint work_dim, size_t * global_work_offset, const size_t *global_work_size, const size_t *local_work_size, cl_uint num_events_in_wait_list)
{
  cl_event event;
  //start kernel
  clFinish(command_queue);
  int err = clEnqueueNDRangeKernel(command_queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size, num_events_in_wait_list, NULL, &event);
  if (err)
  {
    throw OpenCLAlgorithmException("Cant enqueue nd range kernel", err);
  }
  clWaitForEvents(1, &event);

  //calculate time
  cl_ulong start, end;
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(start), &start, NULL);
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(end), &end, NULL);
  total_time = (end - start) / 1000000.0;
  clReleaseEvent(event);
}

double OpenCLAlgorithm::getTimeConsumed() const
{
  return total_time;
}

void OpenCLAlgorithm::prepare(size_t di_size, size_t do_size)
{
  program = device.createAndBuildProgramFromFile(source_file);
 
  cl_int err;
  kernel = clCreateKernel(program, kernel_name.c_str(), &err);
  ASSERT_OPENCL_ERR(err, "Cant create kernel");

  setKernelArgs(di_size, do_size);
}

void OpenCLAlgorithm::setDevice()
{
  command_queue = device.getCommandQueue();
  context = device.getContext();
}

void OpenCLImageAlgorithm::prepareForStream(cl_command_queue cc, cl_context c)
{
  command_queue = cc;
  context = c;

  program = device.createAndBuildProgramFromSource(source);
 
  cl_int err;
  kernel = clCreateKernel(program, kernel_name.c_str(), &err);
  ASSERT_OPENCL_ERR(err, "Cant create kernel");

  setKernelArgsForStream();
}

void OpenCLImageAlgorithm::runStream(const size_t * global_work_size)
{
  copyDataToGPUStream();
  enqueueNDRangeKernelWithTimeMeasurment(2, NULL, global_work_size, NULL, 0);
}
