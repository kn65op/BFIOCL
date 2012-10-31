#pragma once

#include <list>
#include <string>
#include <CL/opencl.h>

#include "OpenCLException.h"

struct OpenCLDeviceException : public OpenCLException
{
  OpenCLDeviceException(std::string m, int err) : OpenCLException(m, err) {}
};


class OpenCLDevice
{
public:
  OpenCLDevice();
  OpenCLDevice(cl_platform_id pid, cl_device_id did);
  ~OpenCLDevice(void);

  static std::list<OpenCLDevice> getDevices();
  
  std::string getName();
  std::string getPlatformName();
  cl_context getContext();
  cl_command_queue getCommandQueue();
  cl_program createAndBuildProgramFromSource(std::string source);
  cl_program createAndBuildProgramFromFile(std::string filename);
  
  
protected:  
  cl_platform_id platform_id;
  cl_device_id device_id;
  
private:
  std::string platform_name;
  std::string device_name;
  cl_context context;
  
};

