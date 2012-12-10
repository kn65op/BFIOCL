#include "OpenCLFloatToInt.h"


OpenCLFloatToInt::OpenCLFloatToInt(void)
{
  source_file = "conversions.cl";
  input = output = NULL;
  kernel_name = "floatToIntThreeChannels";
  input_format.image_channel_order = CL_RGBA;
  input_format.image_channel_data_type = CL_FLOAT;
  output_format.image_channel_order = CL_RGBA;
  output_format.image_channel_data_type= CL_UNSIGNED_INT8;
}


OpenCLFloatToInt::~OpenCLFloatToInt(void)
{
}

void OpenCLFloatToInt::copyDataToGPUStream()
{
  //nothing to do now, before there is no other parameters
}

void OpenCLFloatToInt::setKernelArgsForStream()
{
  //nothing to do now, before there is no other parameters
}

void OpenCLFloatToInt::releaseMem ()
{
  //nothing to do now, before there is no other parameters
}

void OpenCLFloatToInt::setParams (const OpenCLAlgorithmParams& params)
{
  //nothing to do now, before there is no other parameters
}

void OpenCLFloatToInt::run (const unsigned char* data_input, size_t di_size, unsigned char* data_output, size_t do_size)
{
  //nothing to do now, before there is no other parameters
}

void OpenCLFloatToInt::setKernelArgs (size_t di_size, size_t do_size)
{
  //nothing to do now, before there is no other parameters 
}
