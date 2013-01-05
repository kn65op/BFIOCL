#include "OpenCLBayerFilter.h"
#include <iostream>

#define ASSERT_OPENCL_ERR(ERR,MSG) if(ERR != CL_SUCCESS) \
{ \
  throw OpenCLAlgorithmException(MSG, ERR); \
}

const std::string OpenCLBayerFilter::SOURCEFILE = "bayer.cl";

OpenCLBayerFilter::OpenCLBayerFilter()
{
  input = output = kparams = mem_balance = NULL;
  source_file = "bayer.cl";
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

  //set color balance
  balance[0] = params.balance[0];
  balance[1] = params.balance[1];
  balance[2] = params.balance[2];
}

void OpenCLBayerFilter::run(const unsigned char* data_input, size_t di_size, unsigned char* data_output, size_t do_size)
{
  if (params.width == 0 || params.height == 0) throw OpenCLAlgorithmException("Must set image width and height", 0);
 
  copyDataToGPU(data_input, di_size);
  
  //Wykonaj operacje
  enqueueNDRangeKernelWithTimeMeasurment(2, NULL, global_work_size, NULL, 0);
  getResult(data_output, do_size);
}

void OpenCLBayerFilter::releaseMem()
{
  clReleaseMemObject(input);
  clReleaseMemObject(output);
  clReleaseMemObject(kparams);
  clReleaseMemObject(mem_balance);
  input = output = kparams = mem_balance = NULL;
}

void OpenCLBayerFilter::setKernelArgsForStream()
{
 // cl_int err;
    /*
  kparams = clCreateBuffer(context,CL_MEM_READ_ONLY, sizeof(kernel_params),NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer kparams");

  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &kparams);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 0")*/
}

/************** FLOAT *************************/

OpenCLBayerFilterFloat::OpenCLBayerFilterFloat()
{
  kernel_name = "bayer";
  //output_element_size = sizeof(float) * 3;//3 channels
}


void OpenCLBayerFilterFloat::copyDataToGPU(const unsigned char* data_input, size_t di_size)
{
  cl_int err;

  err = clEnqueueWriteBuffer(command_queue, kparams,CL_TRUE,0, sizeof(kernel_params), kernel_params, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer: BayerFilterFloat  kernel params to GPU")
  
  err = clEnqueueWriteBuffer(command_queue, input,CL_TRUE,0, di_size, data_input, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer: BayerFilterFloat input image to GPU");
  
  err = clEnqueueWriteBuffer(command_queue, mem_balance,CL_TRUE,0, sizeof(balance), balance, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer: BayerFilterFloat balance to GPU");
}

void OpenCLBayerFilterFloat::setKernelArgs(size_t di_size, size_t do_size)
{
  releaseMem();

  cl_int err;
    
  kparams = clCreateBuffer(context,CL_MEM_READ_ONLY, sizeof(kernel_params),NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer kparams");
  input = clCreateBuffer(context,CL_MEM_READ_ONLY,di_size,NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer input");
  output = clCreateBuffer(context,CL_MEM_WRITE_ONLY,do_size,NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer output");
  output = clCreateBuffer(context,CL_MEM_WRITE_ONLY,sizeof(balance),NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer balance");

  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &input);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 0")
  
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &output);
  ASSERT_OPENCL_ERR(err,"Cant set kernel arg 1")

  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &kparams);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 2")

  err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*) &balance);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 3")
}


void OpenCLBayerFilterFloat::getResult (unsigned char* data_output, size_t do_size)
{
  cl_int err = clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, do_size, data_output, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err, "Cant enqueue read buffer")
}

/************************* IMAGE ************************************/

OpenCLBayerFilterImage::OpenCLBayerFilterImage(BayerFilterMask mask_type)
{
  switch(mask_type) {
    case BayerFilterMask::CROSS:
      kernel_name = "bayer_image_cross_mask";
      break;
    case BayerFilterMask::CIRCLE:
      kernel_name = "bayer_image_circle_mask";
      break;
    case BayerFilterMask::SQUARE:
    default:
      kernel_name = "bayer_image";
      break;
  }
  input_format.image_channel_order = CL_LUMINANCE;
  input_format.image_channel_data_type = CL_FLOAT;
  output_format.image_channel_order = CL_RGBA;
  output_format.image_channel_data_type= CL_FLOAT;
}

void OpenCLBayerFilterImage::copyDataToGPU(const unsigned char* data_input, size_t di_size)
{
  cl_int err;

  size_t origin[] = {0,0,0};
  size_t region[] = {params.width, params.height, 1};

  err = clEnqueueWriteBuffer(command_queue, kparams, CL_TRUE, 0, sizeof(kernel_params), kernel_params, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer: BayerFilterImage kernel params");
    
  err = clEnqueueWriteImage(command_queue, input, CL_TRUE, origin, region, 0, 0, (void*)data_input, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write image: BayerFilterImage");

  err = clEnqueueWriteBuffer(command_queue, mem_balance, CL_TRUE, 0, sizeof(balance), balance, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write image: BayerFilterImage");
}

void OpenCLBayerFilterImage::setKernelArgs (size_t di_size, size_t do_size)
{
  releaseMem();

  cl_int err;
  
  //create structures
  kparams = clCreateBuffer(context,CL_MEM_READ_ONLY, sizeof(kernel_params),NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer kparams");
    
  input = clCreateImage2D(context, CL_MEM_READ_ONLY, &input_format, params.width, params.height, 0, NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating image2D for input");

  output = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &output_format, params.width, params.height, 0, NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating image2D for output");

  /*sampler = clCreateSampler(context, CL_TRUE, CL_ADDRESS_MIRRORED_REPEAT, CL_FILTER_LINEAR, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating sampler");*/
 
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &input);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 0");
  
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &output);
  ASSERT_OPENCL_ERR(err,"Cant set kernel arg 1");

  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &kparams);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 2");

  err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*) &mem_balance);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 3");
}

void OpenCLBayerFilterImage::releaseMem()
{
  OpenCLBayerFilter::releaseMem();
  //clReleaseSampler(sampler);
}

void OpenCLBayerFilterImage::getResult (unsigned char* data_output, size_t do_size)
{
  size_t origin[] = {0,0,0};
  size_t region[] = {params.width, params.height, 1};
  cl_int err = clEnqueueReadImage(command_queue, output, CL_TRUE, origin, region, 0, 0, (void*)data_output, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err, "Cant enqueue read buffer. BayerFilterImage")
}

void OpenCLBayerFilterImage::copyDataToGPUStream()
{
  cl_int err;

  err = clEnqueueWriteBuffer(command_queue, kparams,CL_TRUE, 0, sizeof(kernel_params), kernel_params, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer: BayerFilterImage for Stream kernel params");

  err = clEnqueueWriteBuffer(command_queue, mem_balance, CL_TRUE, 0, sizeof(balance), balance, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write mem_balance: BayerFilterImage");
}

void OpenCLBayerFilterImage::setKernelArgsForStream()
{
  //TODO: Release memory
  cl_int err;
  
  //create structures
  kparams = clCreateBuffer(context,CL_MEM_READ_ONLY, sizeof(kernel_params),NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer kparams");
  
  mem_balance = clCreateBuffer(context,CL_MEM_READ_ONLY, sizeof(balance),NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer balance");

  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &kparams);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 2 - kparams for BayerFilterImage");

  err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*) &mem_balance);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 2 - kparams for BayerFilterImage");
}