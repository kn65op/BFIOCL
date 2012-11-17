#include "OpenCLBayerFilter.h"
#include <iostream>

#define ASSERT_OPENCL_ERR(ERR,MSG) if(ERR != CL_SUCCESS) \
{ \
  throw OpenCLAlgorithmException(MSG, ERR); \
}

const std::string OpenCLBayerFilter::SOURCEFILE = "bayer.cl";

OpenCLBayerFilter::OpenCLBayerFilter()
{
  input = output = kparams = NULL;
}

OpenCLBayerFilter::~OpenCLBayerFilter()
{
  releaseMem();
}

void OpenCLBayerFilter::setParams(const OpenCLAlgorithmParams& params)
{
  setParams(dynamic_cast<const OpenCLBayerFilterParams&>(params));
}

void OpenCLBayerFilter::setParams(const OpenCLBayerFilterParams& params) {
  this->params = params;
  
  //set work size
  global_work_size[0] = params.width;
  global_work_size[1] = params.height;

  //set other params
  kernel_params[0] = params.pattern;
  kernel_params[1] = (params.mode >> 4) & 0x03;
  kernel_params[2] = (params.mode >> 2) & 0x03;
  kernel_params[3] = params.mode & 0x03;
}

void OpenCLBayerFilter::getResult (unsigned char* data_output, size_t do_size)
{
  cl_int err = clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, do_size, data_output, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err, "Cant enqueue read buffer")
}

void OpenCLBayerFilter::run(const unsigned char* data_input, size_t di_size, unsigned char* data_output, size_t do_size)
{
  if (params.width == 0 || params.height == 0) throw OpenCLAlgorithmException("Must set image width and height", 0);
 
  setKernelArgs(data_input, di_size, do_size);
  //Wykonaj operacje
  enqueueNDRangeKernelWithTimeMeasurment(2, NULL, global_work_size, NULL, 0);
  getResult(data_output, do_size);
  releaseMem();
}

void OpenCLBayerFilter::releaseMem()
{
  clReleaseMemObject(input);
  clReleaseMemObject(output);
  clReleaseMemObject(kparams);
  input = output = kparams = NULL;
}

void OpenCLBayerFilter::prepare()
{
  command_queue = device.getCommandQueue();
  program = device.createAndBuildProgramFromFile(OpenCLBayerFilter::SOURCEFILE);
  cl_int err;
  kernel = clCreateKernel(program, "bayer", &err);
  ASSERT_OPENCL_ERR(err, "Cant create kernel");

}

void OpenCLBayerFilterFloat::setKernelArgs(const unsigned char* data_input, size_t di_size, size_t do_size)
{
  cl_int err;
    
  kparams = clCreateBuffer(device.getContext(),CL_MEM_READ_ONLY, sizeof(kernel_params),NULL, &err);
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
}

void OpenCLBayerFilterImage::setKernelArgs (const unsigned char* data_input, size_t di_size, size_t do_size)
{
  cl_int err;
  cl_image_format input_format;
  cl_image_format output_format;
  output_format.image_channel_data_type = input_format.image_channel_data_type = CL_FLOAT;
  input_format.image_channel_order = CL_INTENSITY;
  input_format.image_channel_data_type = CL_RGB;
  size_t origin[] = {0,0,0};
  size_t region[] = {params.width, params.height, 1};
  
  //create structures
  kparams = clCreateBuffer(device.getContext(),CL_MEM_READ_ONLY, sizeof(kernel_params),NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer kparams");

  input = clCreateImage2D(device.getContext(), CL_MEM_READ_ONLY, &input_format, params.width, params.height, 0, NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating image2D for input");

  input = clCreateImage2D(device.getContext(), CL_MEM_WRITE_ONLY, &output_format, params.width, params.height, 0, NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating image2D for output");

  sampler = clCreateSampler(device.getContext(), CL_TRUE, CL_ADDRESS_MIRRORED_REPEAT, CL_FILTER_LINEAR, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating sampler");

  //wgraj dane
  err = clEnqueueWriteBuffer(command_queue, kparams,CL_TRUE,0, sizeof(kernel_params), kernel_params, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer")

  err = clEnqueueWriteImage(command_queue, input, CL_TRUE, origin, region, 0, 0, (void*)data_input, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write image");

  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &kparams);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 0")
    
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &input);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 1")
  
  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &output);
  ASSERT_OPENCL_ERR(err,"Cant set kernel arg 2")
  //TODO;
}
