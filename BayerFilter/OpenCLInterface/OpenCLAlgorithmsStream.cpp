#include "OpenCLAlgorithmsStream.h"

#include <algorithm>

#define ASSERT_OPENCL_ERR(ERR,MSG) if(ERR != CL_SUCCESS) \
{ \
  throw OpenCLAlgorithmException(MSG, ERR); \
}

OpenCLAlgorithmsStream::OpenCLAlgorithmsStream(void)
{
  width = height = 0;
}


OpenCLAlgorithmsStream::~OpenCLAlgorithmsStream(void)
{
  clearAlgorithms();
}

void OpenCLAlgorithmsStream::pushAlgorithm(OpenCLImageAlgorithm * al)
{
  if (!algorithms.empty() &&
    algorithms.back()->output_format.image_channel_data_type == al->input_format.image_channel_data_type &&
    algorithms.back()->output_format.image_channel_order == al->input_format.image_channel_order) //not first algorithm - need to check if data types for output of last algorithm and input of al is same
  {
    ASSERT_OPENCL_ERR(0, "Wrog data types");
  }
  algorithms.push_back(al);
}

void OpenCLAlgorithmsStream::clearAlgorithms()
{
  auto end = algorithms.end();
  for (auto al = algorithms.begin(); al != end; ++al)
  {
    delete *al;
    *al = 0;
  }
  algorithms.clear();
}

void OpenCLAlgorithmsStream::setDataSize(size_t w, size_t h)
{
  width = w;
  height = h;
}

void OpenCLAlgorithmsStream::prepare()
{//TODO: test if algorithms has at least one algorithm
  //create all kernels
  std::for_each(algorithms.begin(), algorithms.end(), [this](OpenCLImageAlgorithm* al)
  {
    al->setDevice(device);
    al->prepareForStream(command_queue, context);
  });

  //creates input and outputs
  cl_int err;

  //input //TODO:Change to images
  input = clCreateImage2D(context, CL_MEM_READ_ONLY, &algorithms.front()->input_format, width, height, 0, NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating image2D for input");
  
  err = clSetKernelArg(algorithms.front()->kernel, 0, sizeof(cl_mem), (void*) &input);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 0 of first algorithm");
  
  //output
  output = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &algorithms.back()->output_format, width, height, 0, NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating image2D for output");
  
  err = clSetKernelArg(algorithms.back()->kernel, 1, sizeof(cl_mem), (void*) &output);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 1 of last algorithm");

  //middle - starts with first and finish in one before last
  auto end = --algorithms.end();
  auto next = ++algorithms.begin();
  for (auto al = algorithms.begin(); al != end; ++al, ++next)
  {
    cl_mem mem_tmp = clCreateImage2D(context, CL_MEM_READ_WRITE, &(*al)->output_format, width, height, 0, NULL, &err);
    ASSERT_OPENCL_ERR(err, "Error while creating image2D for input/output");

    //set kernel arg for this algorithm
    err = clSetKernelArg((*al)->kernel, 1, sizeof(cl_mem), (void*) &mem_tmp);
    ASSERT_OPENCL_ERR(err, "Cant set kernel arg 0 of middle algorithm: " + (*al)->kernel_name);

    //set kernel arg for next algorithm
    err = clSetKernelArg((*next)->kernel, 0, sizeof(cl_mem), (void*) &mem_tmp);
    ASSERT_OPENCL_ERR(err, "Cant set kernel arg 1 of middle algorithm: " + (*al)->kernel_name);

    //save cl_mem for releasing
    mems.push_back(mem_tmp);
  }
}

void OpenCLAlgorithmsStream::processImage(const void * data_input, void * data_output)
{
  cl_int err;
  time = 0;
  
  size_t origin[] = {0,0,0};
  size_t region[] = {width, height, 1};

  const size_t global_work_size[] = {width, height};
  
  err = clEnqueueWriteImage(command_queue, input, CL_FALSE, origin, region, 0, 0, data_input, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err, "Cant set equeue write input image");

  std::for_each(algorithms.begin(), algorithms.end(), [&global_work_size, this](OpenCLImageAlgorithm *al)
  {
    al->runStream(global_work_size);
    time += al->getTimeConsumed();
  });

  err = clEnqueueReadImage(command_queue, output, CL_TRUE, origin, region, 0, 0, data_output, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err, "Cant enqueue read buffer")
}

void OpenCLAlgorithmsStream::setDevice(OpenCLDevice & d)
{
  if (d.isValid())
  {
    device = d;
    command_queue = device.getCommandQueue();
    context = device.getContext();
    return;
  }
  throw OpenCLAlgorithmException("Invalid Device");
}

double OpenCLAlgorithmsStream::getTime()
{
  return time;
}