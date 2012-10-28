#include "OpenCLDevice.h"

#include <opencl.h>

#include <iostream>

OpenCLDevice::OpenCLDevice(void)
{
}


OpenCLDevice::~OpenCLDevice(void)
{
}

std::list<OpenCLDevice> OpenCLDevice::getDevices()
{
  cl_uint platform_number;
  cl_platform_id * platform_id;
  cl_int err;

  //seaching of platforms available
  if (err = clGetPlatformIDs(0, NULL, &platform_number))
  {
    throw CLDeviceException("Expected error");
  }
  std::cout << platform_number << "\n";

  /*if (err = clGetPlatformIDs(1,  platform_id, &platform_number)
  {

  }*/
  return std::list<OpenCLDevice>();
}