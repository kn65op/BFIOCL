#include "OpenCLIntToFloat.h"


OpenCLIntToFloat::OpenCLIntToFloat(void)
{
  source_file = "conversions.cl";
  input = output = NULL;
  kernel_name = "intToFloatOneChannel";
  input_format.image_channel_order = CL_LUMINANCE;
  input_format.image_channel_data_type = CL_UNSIGNED_INT8;
  output_format.image_channel_order = CL_LUMINANCE;
  output_format.image_channel_data_type= CL_FLOAT;
}


OpenCLIntToFloat::~OpenCLIntToFloat(void)
{
}

void OpenCLIntToFloat::copyDataToGPUStream()
{
  //nothing to do now, before there is no other parameters
}

void OpenCLIntToFloat::setKernelArgsForStream()
{
  //nothing to do now, before there is no other parameters
}

void OpenCLIntToFloat::releaseMem ()
{
  //nothing to do now, before there is no other parameters
}

void OpenCLIntToFloat::setParams (const OpenCLAlgorithmParams& params)
{
  //nothing to do now, before there is no other parameters
}

void OpenCLIntToFloat::run (const unsigned char* data_input, size_t di_size, unsigned char* data_output, size_t do_size)
{
  //nothing to do now, before there is no other parameters
}

void OpenCLIntToFloat::setKernelArgs (size_t di_size, size_t do_size)
{
  //nothing to do now, before there is no other parameters 
}
