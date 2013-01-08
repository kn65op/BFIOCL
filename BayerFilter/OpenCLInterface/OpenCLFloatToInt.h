#pragma once

#include "OpenCLAlgorithm.h"

enum class OpenCLFloatToIntMode
{
  UINT8,
  UINT16,
  UINT32 
};

/**
 * @brief Algorithm performs float to int conversion.
 * 
 * Possible output modes:
 * * OpenCLFloatToIntMode::UINT8 - 8 bit unsigned int
 * * OpenCLFloatToIntMode::UINT16 - 16 bit unsigned int
 * * OpenCLFloatToIntMode::UINT32 - 32 bit unsigned int
 */
class OpenCLFloatToInt : public OpenCLImageAlgorithm
{
public:
  OpenCLFloatToInt(OpenCLFloatToIntMode mode = OpenCLFloatToIntMode::UINT8);
  ~OpenCLFloatToInt(void);
private:
//from OpenCLImageAlgorithm
  void copyDataToGPUStream();
  void setKernelArgsForStream();

  //from OpenCLAlgorithm
  void releaseMem();
  void setParams(const OpenCLAlgorithmParams & params);
  void run(const unsigned char * data_input, size_t di_size, unsigned char * data_output, size_t do_size);
  void setKernelArgs(size_t di_size, size_t do_size);
};


