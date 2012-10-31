#pragma once

#include "OpenCLDevice.h"
#include "OpenCLException.h"

struct OpenCLAlgorithmException : public OpenCLException
{
  OpenCLAlgorithmException(std::string m, int e) : OpenCLException(m,e) {}
};

struct OpenCLAlgorithmParams {};

class OpenCLAlgorithm
{
public:
  OpenCLAlgorithm(void);
  OpenCLAlgorithm(OpenCLDevice dev);
  ~OpenCLAlgorithm(void);

  void setDevice(OpenCLDevice dev);
  void setParams(const OpenCLAlgorithmParams & params) = 0;
  void prepare() = 0;
  void run(const uchar * data_input, size_t di_size, uchar * data_output, size_t do_size) = 0;
  
private:
  OpenCLDevice device;
  cl_kernel kernel;
  cl_program program;  
};

