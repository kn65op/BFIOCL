#include "OpenCLIntToFloat.h"


OpenCLIntToFloat::OpenCLIntToFloat(void)
{
  source_file = "conversions.cl";
  source = "const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;\n"
"\n"
"\n"
"__kernel void  intToFloatOneChannel(__read_only image2d_t input, __write_only image2d_t output) \n"
"{\n"
"  \n"
  "const int i = get_global_id(0); //row\n"
  "const int j = get_global_id(1); //column\n"
"\n"
  "uint4 pixel = read_imageui(input, sampler, (int2)(i,j));\n"
"\n"
  "float4 out;\n"
  "out.s0 = pixel.s0 / 255.0;\n"
"  \n"
  "write_imagef(output, (int2)(i,j), out);\n"
"}\n"
"\n";
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
