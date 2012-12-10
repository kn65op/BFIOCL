#include "OpenCLFloatToInt.h"


OpenCLFloatToInt::OpenCLFloatToInt(OpenCLFloatToIntMode mode)
{
  switch (mode)
  {
    case OpenCLFloatToIntMode::UINT8:
      output_format.image_channel_data_type= CL_UNSIGNED_INT8;
      kernel_name = "floatToUInt8ThreeChannels";
      break;
    case OpenCLFloatToIntMode::UINT16:
      output_format.image_channel_data_type= CL_UNSIGNED_INT16;
      kernel_name = "floatToUInt16ThreeChannels";
      break;
    case OpenCLFloatToIntMode::UINT32:
      output_format.image_channel_data_type= CL_UNSIGNED_INT32;
      kernel_name = "floatToUInt32ThreeChannels";
      break;
    default:
      throw OpenCLAlgorithmException("Something went very wrong, because there is no other options");
  }

  source_file = "conversions.cl";
  input = output = NULL;
  input_format.image_channel_order = CL_RGBA;
  input_format.image_channel_data_type = CL_FLOAT;
  output_format.image_channel_order = CL_RGBA;
}


OpenCLFloatToInt::~OpenCLFloatToInt(void)
{
}

void OpenCLFloatToInt::copyDataToGPUStream()
{
  //nothing to do now, because there is no other parameters
}

void OpenCLFloatToInt::setKernelArgsForStream()
{
  //nothing to do now, because there is no other parameters
}

void OpenCLFloatToInt::releaseMem ()
{
  //nothing to do now, because there is no other parameters
}

void OpenCLFloatToInt::setParams (const OpenCLAlgorithmParams& params)
{
  //nothing to do now, because there is no other parameters
}

void OpenCLFloatToInt::run (const unsigned char* data_input, size_t di_size, unsigned char* data_output, size_t do_size)
{
  //nothing to do now, because there is no other parameters
}

void OpenCLFloatToInt::setKernelArgs (size_t di_size, size_t do_size)
{
  //nothing to do now, because there is no other parameters 
}
