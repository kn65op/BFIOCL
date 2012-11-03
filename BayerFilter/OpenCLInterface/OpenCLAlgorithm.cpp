#include "OpenCLAlgorithm.h"

#include <iostream>

OpenCLAlgorithm::OpenCLAlgorithm(void)
{
  kernel = NULL;
  program = NULL;
}

OpenCLAlgorithm::OpenCLAlgorithm(OpenCLDevice dev)
{
  kernel = NULL;
  program = NULL;
  device = dev;
}

OpenCLAlgorithm::~OpenCLAlgorithm(void)
{
  if (program)  clReleaseProgram(program); 
  if (kernel) clReleaseKernel(kernel); 
}

void OpenCLAlgorithm::setDevice(OpenCLDevice dev)
{
  device = dev;
}
