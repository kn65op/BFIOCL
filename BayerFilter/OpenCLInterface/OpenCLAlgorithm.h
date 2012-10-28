#pragma once

#include "OpenCLDevice.h"

class OpenCLAlgorithm
{
public:
  OpenCLAlgorithm(void);
  ~OpenCLAlgorithm(void);

  void setDevice(OpenCLDevice dev);
public:
  OpenCLDevice device;
};

