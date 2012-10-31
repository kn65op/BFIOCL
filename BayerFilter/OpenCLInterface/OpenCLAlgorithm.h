#pragma once

#include "OpenCLDevice.h"
#include "OpenCLException.h"

struct OpenCLAlgorithmException : public OpenCLException
{
  OpenCLAlgorithmException(std::string m, int e) : OpenCLException(m,e) {}
};

class OpenCLAlgorithm
{
public:
  OpenCLAlgorithm(void);
  ~OpenCLAlgorithm(void);

  void setDevice(OpenCLDevice dev);
public:
  OpenCLDevice device;
};

