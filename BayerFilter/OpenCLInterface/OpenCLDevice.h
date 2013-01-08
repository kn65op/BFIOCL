#pragma once

#include <list>
#include <string>
#include <CL/opencl.h>

#include "OpenCLException.h"

struct OpenCLDeviceException : public OpenCLException
{
  OpenCLDeviceException(std::string m, int err = 0) : OpenCLException(m, err) {}
};

/**
 * @brief Class represents CL Device.
 */
class OpenCLDevice
{
public:
  OpenCLDevice();
  /**
   * Constructor with setting cl_platform_id and cl_device_id.
   * @param pid cl_platform_id .
   * @param didcl_device_id.
   */
  OpenCLDevice(cl_platform_id pid, cl_device_id did);
  OpenCLDevice(const OpenCLDevice & orig);
  ~OpenCLDevice(void);

  /**
   * Returns list of all devices.
   * @return List of all devices in system.
   */
  static std::list<OpenCLDevice> getDevices();
  
  /**
   * Get device name.
   * @return std::string with name;
   */
  std::string getName();
  /**
   * Get platform name.
   * @return std::string with name.
   */
  std::string getPlatformName();
  /**
   * Get context for this device.
   * @return cl_context.
   */
  cl_context getContext();
  /**
   * Get command queue for this device.
   * @return cl_command_queue.
   */
  cl_command_queue getCommandQueue();
  /**
   * Create program from source.
   * @param source Program source in std::string.
   * @return cl_program with program.
   */
  cl_program createAndBuildProgramFromSource(std::string source);
  /**
   * Create program from file.
   * @param filename Name in std::string of file with program source.
   * @return cl_program with program.
   */
  cl_program createAndBuildProgramFromFile(std::string filename);
  /**
   * Checks if device is valid and ready for use it.
   * @return true if yes, fakse otherwise.
   */
  bool isValid() const;
  const OpenCLDevice & operator=(const OpenCLDevice &);
  
  
protected:  
  cl_platform_id platform_id;
  cl_device_id device_id;
  
private:
  std::string platform_name;
  std::string device_name;
  cl_context context;
  cl_command_queue command_queue;  

  void copyFrom(const OpenCLDevice & orig);
  void clean();
};

