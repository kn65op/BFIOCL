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
  
  cl_mem kparams, lut_mem, input, output;
  cl_int err;
  cl_event event;
  
  cl_uchar LUT[12] = { 1, 2, 0,
                       4, 0, 3,
                       3, 0, 4,
                       0, 2, 1 };
  
  
  cl_uchar kernel_params[4];
  kernel_params[0] = params.pattern;
  kernel_params[1] = (params.mode >> 4) & 0x03;
  kernel_params[2] = (params.mode >> 2) & 0x03;
  kernel_params[3] = params.mode & 0x03;
  
//   std::cout << "Pattern : " << (int) kernel_params[0] 
//     << " R_offset: " << (int) kernel_params[1] 
//     << " G_offset: " << (int) kernel_params[2] 
//     << " B_offset: " << (int) kernel_params[3] << "\n";
  
  kparams = clCreateBuffer(device.getContext(),CL_MEM_READ_ONLY, sizeof(cl_uchar) * 4,NULL,NULL);
  lut_mem = clCreateBuffer(device.getContext(),CL_MEM_READ_ONLY, sizeof(cl_uchar) * 12,NULL,NULL);
  input = clCreateBuffer(device.getContext(),CL_MEM_READ_ONLY,di_size,NULL,NULL);
  output = clCreateBuffer(device.getContext(),CL_MEM_WRITE_ONLY,do_size,NULL,NULL);
  
  //Wgraj dane
  err = clEnqueueWriteBuffer(command_queue, lut_mem,CL_TRUE,0, sizeof(cl_uchar)*12, LUT, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer");

  err = clEnqueueWriteBuffer(command_queue, kparams,CL_TRUE,0, sizeof(kernel_params), kernel_params, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer")
  
  err = clEnqueueWriteBuffer(command_queue, input,CL_TRUE,0, di_size, data_input, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer");

  /*err = clEnqueueWriteBuffer(command_queue, kparams,CL_TRUE,0, sizeof(kernel_params), kernel_params, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer")*/
    
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &kparams);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 0")
  
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &lut_mem);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 0")
  
  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &input);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 1")
  
  err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*) &output);
  ASSERT_OPENCL_ERR(err,"Cant set kernel arg 2")
  
  
  err = clSetKernelArg(kernel, 4, 8*sizeof(cl_float), NULL);
  err = clSetKernelArg(kernel, 5, 5*sizeof(cl_float), NULL);

  
  //Wykonaj operacje
  size_t global_work_size[2];
  global_work_size[0] = params.width;
  global_work_size[1] = params.height;
  
  size_t local_work_size[2];
  local_work_size[0] = 1;
  local_work_size[1] = 1;
  err = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size, local_work_size,0, NULL, NULL);
  ASSERT_OPENCL_ERR(err, "Cant enqueue nd range kernel")

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

