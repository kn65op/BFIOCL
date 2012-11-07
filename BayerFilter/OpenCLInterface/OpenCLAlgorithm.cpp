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
