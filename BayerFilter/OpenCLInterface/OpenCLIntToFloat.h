#pragma once

#include "OpenCLAlgorithm.h"

/**
 * Enum for selecting conversion.
 */
enum class OpenCLIntToFloatMode
{
  UINT_8,
  UINT_10,
  UINT_12,
  UINT_16
};

/**
 * Algorithm perfoming int to float conversion.
 */
class OpenCLIntToFloat : public OpenCLImageAlgorithm
{
public:
  OpenCLIntToFloat(OpenCLIntToFloatMode mode = OpenCLIntToFloatMode::UINT_8);
  ~OpenCLIntToFloat(void);
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

