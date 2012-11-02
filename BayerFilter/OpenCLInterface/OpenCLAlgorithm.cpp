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
}

void OpenCLAlgorithm::setDevice(OpenCLDevice dev)
{
  device = dev;
}
