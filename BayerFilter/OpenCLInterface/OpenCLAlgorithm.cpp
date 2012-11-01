#include "OpenCLAlgorithm.h"

#include <iostream>

OpenCLAlgorithm::OpenCLAlgorithm(void)
{
  
}

OpenCLAlgorithm::OpenCLAlgorithm(OpenCLDevice dev)
{
  device = dev;
}

OpenCLAlgorithm::~OpenCLAlgorithm(void)
{
}

void OpenCLAlgorithm::setDevice(const OpenCLDevice & dev)
{
  device = dev;
}
