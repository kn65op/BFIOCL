#include "OpenCLAlgorithm.h"

#include <iostream>

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
    return;
  }
  throw OpenCLAlgorithmException("Invalid Device");
}

void OpenCLAlgorithm::enqueueNDRangeKernelWithTimeMeasurment(cl_uint work_dim, size_t * global_work_offset, const size_t *global_work_size, const size_t *local_work_size, cl_uint num_events_in_wait_list)
{
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
}

double OpenCLAlgorithm::getTimeConsumed() const
{
  return total_time;
}