#include "OpenCLBayerFilter.h"
#include <iostream>

#define ASSERT_OPENCL_ERR(ERR,MSG) if(ERR != CL_SUCCESS) \
{ \
  throw OpenCLAlgorithmException(MSG, ERR); \
}

const std::string OpenCLBayerFilter::SOURCEFILE = "bayer.cl";

void OpenCLBayerFilter::setParams(const OpenCLAlgorithmParams& params)
{
  setParams(dynamic_cast<const OpenCLBayerFilterParams&>(params));
}

void OpenCLBayerFilter::setParams(const OpenCLBayerFilterParams& params) {
  this->params = params;
}

void OpenCLBayerFilter::run(const unsigned char* data_input, size_t di_size, unsigned char* data_output, size_t do_size)
{
  if (params.width == 0 || params.height == 0) throw OpenCLAlgorithmException("Must set image width and height", 0);
  
  cl_mem kparams, input, output;
  cl_int err;
    
  cl_uchar kernel_params[4];
  kernel_params[0] = params.pattern;
  kernel_params[1] = (params.mode >> 4) & 0x03;
  kernel_params[2] = (params.mode >> 2) & 0x03;
  kernel_params[3] = params.mode & 0x03;
    
  kparams = clCreateBuffer(device.getContext(),CL_MEM_READ_ONLY, sizeof(cl_uchar) * 4,NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer kparams");
  input = clCreateBuffer(device.getContext(),CL_MEM_READ_ONLY,di_size,NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer input");
  output = clCreateBuffer(device.getContext(),CL_MEM_WRITE_ONLY,do_size,NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer output");
  
  //Wgraj dane
  err = clEnqueueWriteBuffer(command_queue, kparams,CL_TRUE,0, sizeof(kernel_params), kernel_params, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer")
  
  err = clEnqueueWriteBuffer(command_queue, input,CL_TRUE,0, di_size, data_input, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer");
    
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &kparams);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 0")
    
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &input);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 1")
  
  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &output);
  ASSERT_OPENCL_ERR(err,"Cant set kernel arg 2")


  //Wykonaj operacje
  size_t global_work_size[2];
  global_work_size[0] = params.width;
  global_work_size[1] = params.height;
  
  enqueueNDRangeKernelWithTimeMeasurment(2, NULL, global_work_size, NULL, 0);
  
  //Odczytaj dane
  err = clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, do_size, data_output, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err, "Cant enqueue read buffer")
  
  clReleaseMemObject(input);
  clReleaseMemObject(output);
}

void OpenCLBayerFilter::prepare()
{
  command_queue = device.getCommandQueue();
  program = device.createAndBuildProgramFromFile(OpenCLBayerFilter::SOURCEFILE);
  cl_int err;
  kernel = clCreateKernel(program, "bayer", &err);
  ASSERT_OPENCL_ERR(err, "Cant create kernel");

}

