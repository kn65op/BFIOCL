#include "OpenCLAlgorithmsStream.h"

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
  if (!algorithms.empty() && algorithms.back()->output_element_size == al->input_element_size) //not first algorithm - need to check if data types for output of last algorithm and input of al is same
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
{
  std::for_each(
}