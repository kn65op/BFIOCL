
#include "OpenCLInvertImage.h"
#include <iostream>

#define ASSERT_OPENCL_ERR(ERR,MSG) if(ERR != CL_SUCCESS) \
{ \
  throw OpenCLAlgorithmException(MSG, ERR); \
}

const std::string OpenCLInvertImage::SOURCE = "\
__kernel void  invert( __global float* input, __global float* output) \
{ \
  int i = get_global_id(0); \
  output[i] = 1 - input[i]; \
}";

OpenCLInvertImage::~OpenCLInvertImage()
{
  if (program)  clReleaseProgram(program); 
  if (kernel) clReleaseKernel(kernel); 
}


void OpenCLInvertImage::run(const unsigned char* data_input, size_t di_size, unsigned char* data_output, size_t do_size)
{
  cl_mem input, output;
  cl_int err;
  cl_event event;
  input = clCreateBuffer(device.getContext(),CL_MEM_READ_ONLY,di_size,NULL,NULL);
  output = clCreateBuffer(device.getContext(),CL_MEM_WRITE_ONLY,do_size,NULL,NULL);
  
  //Wgraj dane
  err = clEnqueueWriteBuffer(command_queue, input,CL_TRUE,0, di_size, data_input, 0, NULL, &event);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer")
  
  err = clReleaseEvent(event);
  ASSERT_OPENCL_ERR(err, "Cant release event")
    
  err = clSetKernelArg(kernel,0,sizeof(cl_mem), (void*) &input);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 0")
  
  err = clSetKernelArg(kernel,1,sizeof(cl_mem), (void*) &output);
  ASSERT_OPENCL_ERR(err,"Cant set kernel arg 1")
  
  err = clFinish(command_queue);
  ASSERT_OPENCL_ERR(err, "Cant finish queue");
  
  //Wykonaj operacje
  size_t global_work_size = di_size;
  err = clEnqueueNDRangeKernel(command_queue, kernel,1, NULL, &global_work_size, NULL,0, NULL, &event);
  ASSERT_OPENCL_ERR(err, "Cant enqueue nd range kernel")
  
  err = clReleaseEvent(event);
  ASSERT_OPENCL_ERR(err, "Cant release event")
  
  //err = clFinish(device.getCommandQueue());
  //ASSERT_OPENCL_ERR(err, "Cant finish queue");
  
  
  //Odczytaj dane
  err = clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, do_size, data_output, 0, NULL, &event);
  ASSERT_OPENCL_ERR(err, "Cant enqueue read buffer")
  
  err = clReleaseEvent(event);
  ASSERT_OPENCL_ERR(err, "Cant release event")
  
  err = clFinish(command_queue);
  ASSERT_OPENCL_ERR(err, "Cant finish queue");
  
  clReleaseMemObject(input);
  clReleaseMemObject(output);
}

void OpenCLInvertImage::prepare()
{
  command_queue = device.getCommandQueue();
  program = device.createAndBuildProgramFromSource(OpenCLInvertImage::SOURCE);
  cl_int err;
  kernel = clCreateKernel(program, "invert", &err);
  ASSERT_OPENCL_ERR(err, "Cant create kernel");
}

void OpenCLInvertImage::setParams(const OpenCLAlgorithmParams& params)
{

}

