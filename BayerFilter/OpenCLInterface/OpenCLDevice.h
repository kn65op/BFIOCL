#pragma once

#include <list>

#include <opencl.h>

struct CLDeviceException
{
  CLDeviceException(std::string m) : message(m) {}
  std::string message;
};


class OpenCLDevice
{
public:
  OpenCLDevice(void);
  ~OpenCLDevice(void);

  static std::list<OpenCLDevice> getDevices();
};

