
#include "OpenCLDevice.h"

#include <CL/opencl.h>
#include <fstream>
#include <iostream>
#include <sstream>

OpenCLDevice::OpenCLDevice()
{
  this->platform_id = NULL;
  this->device_id = NULL;
  this->context = NULL;
}

OpenCLDevice::OpenCLDevice(cl_platform_id pid, cl_device_id did)
{
  this->platform_id = pid;
  this->device_id = did;
  this->context = NULL;
}


OpenCLDevice::~OpenCLDevice(void)
{
}

std::string OpenCLDevice::getName()
{
  
  if (this->device_name.empty())
  {
    char * chBuffer = new char[100];
    int err;
    if (CL_SUCCESS != (err = clGetDeviceInfo(this->device_id,CL_DEVICE_NAME,100,chBuffer, NULL)))
    {
      throw OpenCLDeviceException("Can't get device name", err);
    }
    this->device_name = std::string(chBuffer);
    delete[] chBuffer;
  }
  return this->device_name;
}

std::string OpenCLDevice::getPlatformName()
{
  if (this->platform_name.empty())
  {
    char * chBuffer = new char[100];
    int err;
    if (CL_SUCCESS != (err = clGetPlatformInfo(this->platform_id, CL_PLATFORM_NAME, 100, chBuffer, NULL)))
    {
      throw OpenCLDeviceException("Can't get platform name", err);
    }
    this->platform_name = std::string(chBuffer);
    delete[] chBuffer;
  }
  return this->platform_name;
}

cl_context OpenCLDevice::getContext()
{
  if (this->context == NULL)
  {
    cl_context_properties prop[3];
    prop[0] = CL_CONTEXT_PLATFORM;
    prop[1] = (cl_context_properties) this->platform_id;
    prop[3] = 0;
    cl_int err;
    this->context = clCreateContext(prop, 1, &(this->device_id), NULL, NULL, &err);
    if (err != CL_SUCCESS) {
      throw OpenCLDeviceException("Can't create context for this device", err);
    }
  }  
  return this->context;
}

cl_program OpenCLDevice::createAndBuildProgramFromFile(std::string filename)
{
  std::ifstream file(filename.c_str());
  std::stringstream content_stream;
  content_stream << file.rdbuf();
  return this->createAndBuildProgramFromSource(content_stream.str());
}

cl_program OpenCLDevice::createAndBuildProgramFromSource(std::string source)
{
  return NULL;
}

cl_command_queue OpenCLDevice::getCommandQueue()
{
  return NULL;
}

std::list<OpenCLDevice> OpenCLDevice::getDevices()
{
  cl_uint platform_number, device_number, i, j;
  cl_platform_id * platform_id;
  cl_device_id * device_id;
  cl_int err;
  
  std::list<OpenCLDevice> devices_list;

  //seaching of platforms available
  if (err = clGetPlatformIDs(0, NULL, &platform_number))
  {
    throw OpenCLDeviceException("Can't get number of OpenCL platforms", err);
  }
  
  platform_id = new cl_platform_id[platform_number];

  if (err = clGetPlatformIDs(platform_number,  platform_id, NULL) )
  {
    throw OpenCLDeviceException("Can't get OpenCL platforms ids", err);
  }
  //std::cout << "Platforms: " << platform_number << "\n";
  for(i=0;i<platform_number;++i) 
  {
    if( err = clGetDeviceIDs(platform_id[i],CL_DEVICE_TYPE_ALL, 0, NULL, &device_number) )
    {
      throw OpenCLDeviceException("Can't get number of devices on platform", err);
    }
    
    device_id = new cl_device_id[device_number];
    
    if( err = clGetDeviceIDs(platform_id[i],CL_DEVICE_TYPE_ALL, device_number, device_id, NULL) )
    {
      throw OpenCLDeviceException("Can't get ids of devices on platform", err);
    }
    //std::cout << "Devices: " << device_number << "\n";
    for(j=0;j<device_number;++j)
    {
      devices_list.push_back(OpenCLDevice(platform_id[i], device_id[j]));
    }
    
    delete[] device_id;
  }
  
  delete[] platform_id;
  return devices_list;
}