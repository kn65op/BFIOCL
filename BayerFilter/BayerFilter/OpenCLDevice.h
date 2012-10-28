#pragma once

#include <list>

#include <opencl.h>

class OpenCLDevice
{
public:
  OpenCLDevice(void);
  ~OpenCLDevice(void);

  static std::list<OpenCLDevice> getDevices();
};

