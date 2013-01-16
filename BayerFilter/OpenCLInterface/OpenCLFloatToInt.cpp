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

  //common
  source_file = "conversions.cl";
  source = "const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;\n"
    "__kernel void  floatToUInt8ThreeChannels(__read_only image2d_t input, __write_only image2d_t output) \n"
"{\n"
"  \n"
  "const int i = get_global_id(0); //row\n"
  "const int j = get_global_id(1); //column\n"
"\n"
  "float4 pixel = read_imagef(input, sampler, (int2)(i,j));\n"
"\n"
  "uint4 out = convert_uint4(pixel * 255.0);\n"
"  \n"
  "write_imageui(output, (int2)(i,j), out);\n"
"}\n"
"\n"
"__kernel void  floatToUInt16ThreeChannels(__read_only image2d_t input, __write_only image2d_t output) \n"
"{\n"
"  \n"
  "const int i = get_global_id(0); //row\n"
  "const int j = get_global_id(1); //column\n"
"\n"
  "float4 pixel = read_imagef(input, sampler, (int2)(i,j));\n"
"\n"
  "uint4 out = convert_uint4(pixel * 65535.0);\n"
"  \n"
  "write_imageui(output, (int2)(i,j), out);\n"
"}\n"
"\n"
"__kernel void  floatToUInt32ThreeChannels(__read_only image2d_t input, __write_only image2d_t output) \n"
"{\n"
"  \n"
  "const int i = get_global_id(0); //row\n"
  "const int j = get_global_id(1); //column\n"
"\n"
  "float4 pixel = read_imagef(input, sampler, (int2)(i,j));\n"
"\n"
  "uint4 out = convert_uint4(pixel * 4294967295.0);\n"
"  \n"
  "write_imageui(output, (int2)(i,j), out);\n"
"}\n";
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
