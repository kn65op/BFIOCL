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
  virtual void setParams(const OpenCLAlgorithmParams & params) = 0;
  virtual void prepare() = 0;
  virtual void run(const unsigned char * data_input, size_t di_size, unsigned char * data_output, size_t do_size) = 0;
  
protected:
  OpenCLDevice device;
 
  cl_kernel kernel;
  cl_program program;  
  cl_command_queue command_queue;
};

