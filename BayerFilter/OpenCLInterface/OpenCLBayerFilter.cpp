#include "OpenCLBayerFilter.h"
#include <iostream>

#define ASSERT_OPENCL_ERR(ERR,MSG) if(ERR != CL_SUCCESS) \
{ \
  throw OpenCLAlgorithmException(MSG, ERR); \
}

const std::string OpenCLBayerFilter::SOURCEFILE = "bayer.cl";

OpenCLBayerFilter::OpenCLBayerFilter()
{
  input = output = kparams = mem_balance = NULL;
  source_file = "bayer.cl";
  source = "__kernel void  bayer(__global float* input, __global float* output, __constant uchar * params, __global float* balance) \n"
"{\n"
"  int width_output = get_global_size(0); \n"
  "int width = get_global_size(0) + 2; \n"
"\n"
"//  int height = get_global_size(1); \n"
  "int jout = get_global_id(0); //column number\n"
  "int j = get_global_id(0) + 1; //column number\n"
  "int i = get_global_id(1) + 1; //row number\n"
  "int iout = get_global_id(1); \n"
  "int ptr = i * width + j; \n"
  "int ptro = iout * width_output+ jout; \n"
  "uchar offset_type = params[0]; \n"
  "uchar pixel_offset_type = ( (offset_type & 0x02) + (iout%2 << 1) + (offset_type + j+1)%2) %4;\n"
  "uchar r_pos = params[1]; \n"
  "uchar g_pos = params[2]; \n"
  "uchar b_pos = params[3]; \n"
"\n"
  "float8 P;\n"
"  \n"
  "P.s0 = input[(i-1)*width + j -1];\n"
  "P.s1 = input[(i-1)*width + j];\n"
  "P.s2 = input[(i-1)*width + j+1];\n"
  "P.s3 = input[(i)*width + j-1];\n"
  "P.s4 = input[(i)*width + j+1];\n"
  "P.s5 = input[(i+1)*width + j-1];\n"
  "P.s6 = input[(i+1)*width + j];\n"
  "P.s7 = input[(i+1)*width + j+1];\n"
"\n"
  "if (pixel_offset_type == 0)\n"
  "{\n"
	  "output[3*ptro + r_pos] = (P.s0 + P.s2 + P.s5 + P.s7) / 4.0 * balance[0];\n"
	  "output[3*ptro + g_pos] = (P.s1 + P.s3 + P.s4 + P.s6) / 4.0 * balance[1];\n"
	  "output[3*ptro + b_pos] = input[ptr] * balance[2];\n"
  "}\n"
  "else if (pixel_offset_type == 1)\n"
  "{\n"
	  "output[3*ptro + r_pos] = (P.s1 + P.s6) / 2.0 * balance[0];\n"
	  "output[3*ptro + g_pos] = input[ptr] * balance[1];\n"
	  "output[3*ptro + b_pos] = (P.s3 + P.s4) / 2.0 * balance[2];\n"
  "}\n"
  "else if (pixel_offset_type == 2)\n"
  "{\n"
	  "output[3*ptro + r_pos] = (P.s3 + P.s4) / 2.0 * balance[0];\n"
	  "output[3*ptro + g_pos] = input[ptr] * balance[1];\n"
	  "output[3*ptro + b_pos] = (P.s1 + P.s6) / 2.0 * balance[2];\n"
  "}\n"
  "else\n"
  "{\n"
	  "output[3*ptro + r_pos] = input[ptr] * balance[0];\n"
	  "output[3*ptro + g_pos] = (P.s1 + P.s3 + P.s4 + P.s6) / 4.0 * balance[1];\n"
	  "output[3*ptro + b_pos] = (P.s0 + P.s2 + P.s5 + P.s7) / 4.0 * balance[2];\n"
  "}\n"
"  \n"
"}\n"
"\n"
"const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_MIRRORED_REPEAT | CLK_FILTER_NEAREST;\n"
"  \n"
"__kernel void  bayer_image(__read_only image2d_t input, __write_only image2d_t output, __constant uchar * params, __global\n" "float* balance) \n"
"{\n"
"  \n"
  "const int i = get_global_id(0); //row\n"
  "const int j = get_global_id(1); //column\n"
  "const uchar offset_type = params[0];\n"
  "const uchar pixel_offset_type = ( (offset_type & 0x02) + (i%2 << 1) + (offset_type + j)%2) %4;\n"
"  \n"
  "// pixel, p_tl, p_tt, p_tr, p_ll, p_rr, p_bl, p_bb, p_br, , \n"
  "float green, red, blue;\n"
  "float4 pixel = read_imagef(input, sampler, (int2)(i,j));\n"
"\n"
  "float4 p_tt = read_imagef(input, sampler, (int2)(i-1,j  ));\n"
  "float4 p_bb = read_imagef(input, sampler, (int2)(i+1,j));\n"
  "float4 p_ll = read_imagef(input, sampler, (int2)(i  ,j-1));\n"
  "float4 p_rr = read_imagef(input, sampler, (int2)(i  ,j+1));\n"
"  \n"
  "float4 p_tl = read_imagef(input, sampler, (int2)(i-1,j-1));\n"
  "float4 p_tr = read_imagef(input, sampler, (int2)(i-1,j+1));\n"
  "float4 p_bl = read_imagef(input, sampler, (int2)(i+1,j-1));\n"
  "float4 p_br = read_imagef(input, sampler, (int2)(i+1,j+1));\n"
"    \n"
  "if (pixel_offset_type == 0)\n"
  "{\n"
      "red = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;\n"
      "green = (p_tt.s0 + p_ll.s0 + p_rr.s0 + p_bb.s0) / 4.0;\n"
      "blue = pixel.s0;\n"
  "}\n"
  "else if (pixel_offset_type == 1)\n"
  "{\n"
      "red = (p_tt.s0 + p_bb.s0) / 2.0;\n"
      "green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 5.0;\n"
      "blue = (p_ll.s0 + p_rr.s0) / 2.0;\n"
  "}\n"
  "else if (pixel_offset_type == 2)\n"
  "{\n"
      "red = (p_ll.s0 + p_rr.s0) / 2.0;\n"
      "green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 5.0;\n"
      "blue = (p_tt.s0 + p_bb.s0) / 2.0;\n"
  "}\n"
  "else\n"
  "{\n"
      "red = pixel.s0;\n"
      "green = (p_tt.s0 + p_bb.s0 + p_ll.s0 + p_rr.s0) / 4.0;\n"
      "blue = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;\n"
  "}\n"
  "pixel.s0 = blue * balance[2];\n"
  "pixel.s1 = green * balance[1];\n"
  "pixel.s2 = red * balance[0];\n"
"  \n"
  "write_imagef(output, (int2)(i,j), pixel);\n"
"}\n"
"\n"
"/**\n"
 "* bayer_image_circle_mask\n"
 "* \n"
 "* Interpolate BayerMatrix output to RGB image using circle mask\n"
 "* \n"
 "*   #\n"
 "*  ###\n"
 "* #####\n"
 "*  ###\n"
 "*   #\n"
 "* \n"
 "*/\n"
"__kernel void  bayer_image_circle_mask(__read_only image2d_t input, __write_only image2d_t output, __constant uchar * params, __global float* balance) \n"
"{\n"
"  \n"
  "const int i = get_global_id(0); //row\n"
  "const int j = get_global_id(1); //column\n"
  "const uchar offset_type = params[0];\n"
  "const uchar pixel_offset_type = ( (offset_type & 0x02) + (i%2 << 1) + (offset_type + j)%2) %4;\n"
"  \n"
  "// pixel, p_tl, p_tt, p_tr, p_ll, p_rr, p_bl, p_bb, p_br, , \n"
  "float green, red, blue;\n"
  "float4 pixel = read_imagef(input, sampler, (int2)(i,j));\n"
"\n"
  "float4 p_tt = read_imagef(input, sampler, (int2)(i-1,j  ));\n"
  "float4 p_bb = read_imagef(input, sampler, (int2)(i+1,j));\n"
  "float4 p_ll = read_imagef(input, sampler, (int2)(i  ,j-1));\n"
  "float4 p_rr = read_imagef(input, sampler, (int2)(i  ,j+1));\n"
"  \n"
  "float4 p_ttt = read_imagef(input, sampler, (int2)(i-2,j  ));\n"
  "float4 p_bbb = read_imagef(input, sampler, (int2)(i+2,j));\n"
  "float4 p_lll = read_imagef(input, sampler, (int2)(i  ,j-2));\n"
  "float4 p_rrr = read_imagef(input, sampler, (int2)(i  ,j+2));\n"
"  \n"
  "float4 p_tl = read_imagef(input, sampler, (int2)(i-1,j-1));\n"
  "float4 p_tr = read_imagef(input, sampler, (int2)(i-1,j+1));\n"
  "float4 p_bl = read_imagef(input, sampler, (int2)(i+1,j-1));\n"
  "float4 p_br = read_imagef(input, sampler, (int2)(i+1,j+1));\n"
"  \n"
  "// 0 -> tl\n"
  "// 1 -> tt\n"
  "// 2 -> tr\n"
  "// 3 -> ll\n"
  "// 4 -> rr\n"
  "// 5 -> bl\n"
  "// 6 -> bb\n"
  "// 7 -> br\n"
"  \n"
  "if (pixel_offset_type == 0)\n"
  "{\n"
      "red = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;\n"
      "green = (p_tt.s0 + p_ll.s0 + p_rr.s0 + p_bb.s0) / 4.0;\n"
      "blue = (pixel.s0 + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 5.0;\n"
  "}\n"
  "else if (pixel_offset_type == 1)\n"
  "{\n"
      "red = (p_tt.s0 + p_bb.s0) / 2.0;\n"
      "green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0  + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 9.0;\n"
      "blue = (p_ll.s0 + p_rr.s0) / 2.0;\n"
  "}\n"
  "else if (pixel_offset_type == 2)\n"
  "{\n"
      "red = (p_ll.s0 + p_rr.s0) / 2.0;\n"
      "green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0  + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 9.0;\n"
      "blue = (p_tt.s0 + p_bb.s0) / 2.0;\n"
  "}\n"
  "else\n"
  "{\n"
      "red = (pixel.s0 + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 5.0;\n"
      "green = (p_tt.s0 + p_bb.s0 + p_ll.s0 + p_rr.s0) / 4.0;\n"
      "blue = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;\n"
  "}\n"
  "pixel.s0 = blue * balance[2];\n"
  "pixel.s1 = green * balance[1];\n"
  "pixel.s2 = red * balance[0];\n"
"  \n"
  "write_imagef(output, (int2)(i,j), pixel);\n"
"}\n"
"\n"
"/**\n"
 "* bayer_image_cross_mask\n"
 "* \n"
 "* Interpolate BayerMatrix output to RGB image using cross mask:\n"
 "* \n"
 "*  ###\n"
 "* #####\n"
 "* #####\n"
 "* #####\n"
 "*  ###\n"
 "* \n"
 "*/\n"
"__kernel void  bayer_image_cross_mask(__read_only image2d_t input, __write_only image2d_t output, __constant uchar * params, __global float* balance) \n"
"{\n"
"  \n"
  "const int i = get_global_id(0); //row\n"
  "const int j = get_global_id(1); //column\n"
  "const uchar offset_type = params[0];\n"
  "const uchar pixel_offset_type = ( (offset_type & 0x02) + (i%2 << 1) + (offset_type + j)%2) %4;\n"
"  \n"
  "// pixel, p_tl, p_tt, p_tr, p_ll, p_rr, p_bl, p_bb, p_br, , \n"
  "float green, red, blue;\n"
  "float4 pixel = read_imagef(input, sampler, (int2)(i,j));\n"
"\n"
  "float4 p_tt = read_imagef(input, sampler, (int2)(i-1,j  ));\n"
  "float4 p_bb = read_imagef(input, sampler, (int2)(i+1,j));\n"
  "float4 p_ll = read_imagef(input, sampler, (int2)(i  ,j-1));\n"
  "float4 p_rr = read_imagef(input, sampler, (int2)(i  ,j+1));\n"
"  \n"
  "float4 p_ttt = read_imagef(input, sampler, (int2)(i-2,j  ));\n"
  "float4 p_bbb = read_imagef(input, sampler, (int2)(i+2,j));\n"
  "float4 p_lll = read_imagef(input, sampler, (int2)(i  ,j-2));\n"
  "float4 p_rrr = read_imagef(input, sampler, (int2)(i  ,j+2));\n"
"  \n"
  "float4 p_tl = read_imagef(input, sampler, (int2)(i-1,j-1));\n"
  "float4 p_tr = read_imagef(input, sampler, (int2)(i-1,j+1));\n"
  "float4 p_bl = read_imagef(input, sampler, (int2)(i+1,j-1));\n"
  "float4 p_br = read_imagef(input, sampler, (int2)(i+1,j+1));\n"
"  \n"
"  \n"
  "float4 p_ttl = read_imagef(input, sampler, (int2)(i-2,j-1));\n"
  "float4 p_ttr = read_imagef(input, sampler, (int2)(i-2,j+1));\n"
"  \n"
  "float4 p_bbl = read_imagef(input, sampler, (int2)(i+2,j-1));\n"
  "float4 p_bbr = read_imagef(input, sampler, (int2)(i+2,j+1));\n"
"  \n"
  "float4 p_llt = read_imagef(input, sampler, (int2)(i-1,j-2));\n"
  "float4 p_llb = read_imagef(input, sampler, (int2)(i+1,j-2));\n"
"\n"
  "float4 p_rrt = read_imagef(input, sampler, (int2)(i-1,j+2));\n"
  "float4 p_rrb = read_imagef(input, sampler, (int2)(i+1,j+2));\n"
"  \n"
  "if (pixel_offset_type == 0)\n"
  "{\n"
      "red = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;\n"
      "green = (p_tt.s0 + p_ll.s0 + p_rr.s0 + p_bb.s0 + p_ttl.s0 + p_ttr.s0 + p_bbl.s0 + p_bbr.s0 + p_llt.s0 + p_llb.s0 +\n" "p_rrt.s0 + p_rrb.s0) / 12.0;\n"
      "blue = (pixel.s0 + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 5.0;\n"
  "}\n"
  "else if (pixel_offset_type == 1)\n"
  "{\n"
      "red = (p_tt.s0 + p_bb.s0 + p_llt.s0 + p_llb.s0 + p_rrt.s0 + p_rrb.s0) / 6.0;\n"
      "green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0  + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 9.0;\n"
      "blue = (p_ll.s0 + p_rr.s0 + p_ttl.s0 + p_ttr.s0 + p_bbl.s0 + p_bbr.s0) / 6.0;\n"
  "}\n"
  "else if (pixel_offset_type == 2)\n"
  "{\n"
      "red = (p_ll.s0 + p_rr.s0 + p_ttl.s0 + p_ttr.s0 + p_bbl.s0 + p_bbr.s0) / 6.0;\n"
      "green = (pixel.s0 + p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0  + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 9.0;\n"
      "blue = (p_tt.s0 + p_bb.s0 + p_llt.s0 + p_llb.s0 + p_rrt.s0 + p_rrb.s0) / 6.0;\n"
  "}\n"
  "else\n"
  "{\n"
      "red = (pixel.s0 + p_ttt.s0 + p_lll.s0 + p_rrr.s0 + p_bbb.s0) / 5.0;\n"
      "green = (p_tt.s0 + p_ll.s0 + p_rr.s0 + p_bb.s0 + p_ttl.s0 + p_ttr.s0 + p_bbl.s0 + p_bbr.s0 + p_llt.s0 + p_llb.s0 +\n" "p_rrt.s0 + p_rrb.s0) / 12.0;\n"
      "blue = (p_tl.s0 + p_tr.s0 + p_bl.s0 + p_br.s0) / 4.0;\n"
  "}\n"
"  \n"
  "pixel.s0 = blue * balance[2];\n"
  "pixel.s1 = green * balance[1];\n"
  "pixel.s2 = red * balance[0];\n"
"  \n"
  "write_imagef(output, (int2)(i,j), pixel);\n"
"}\n"
"\n";
}

OpenCLBayerFilter::~OpenCLBayerFilter()
{
  releaseMem();
}

void OpenCLBayerFilter::setParams(const OpenCLAlgorithmParams& params)
{
  setParams(dynamic_cast<const OpenCLBayerFilterParams&>(params));
}

void OpenCLBayerFilter::setParams(const OpenCLBayerFilterParams& params) {
  this->params = params;
  
  //set work size
  global_work_size[0] = params.width;
  global_work_size[1] = params.height;

  //set other params
  kernel_params[0] = params.pattern;
  kernel_params[1] = (params.mode >> 4) & 0x03;
  kernel_params[2] = (params.mode >> 2) & 0x03;
  kernel_params[3] = params.mode & 0x03;

  //set color balance
  balance[0] = params.balance[0];
  balance[1] = params.balance[1];
  balance[2] = params.balance[2];
}

void OpenCLBayerFilter::run(const unsigned char* data_input, size_t di_size, unsigned char* data_output, size_t do_size)
{
  if (params.width == 0 || params.height == 0) throw OpenCLAlgorithmException("Must set image width and height", 0);
 
  copyDataToGPU(data_input, di_size);
  
  //Wykonaj operacje
  enqueueNDRangeKernelWithTimeMeasurment(2, NULL, global_work_size, NULL, 0);
  getResult(data_output, do_size);
}

void OpenCLBayerFilter::releaseMem()
{
  clReleaseMemObject(input);
  clReleaseMemObject(output);
  clReleaseMemObject(kparams);
  clReleaseMemObject(mem_balance);
  input = output = kparams = mem_balance = NULL;
}

void OpenCLBayerFilter::setKernelArgsForStream()
{
 // cl_int err;
    /*
  kparams = clCreateBuffer(context,CL_MEM_READ_ONLY, sizeof(kernel_params),NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer kparams");

  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &kparams);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 0")*/
}

/************** FLOAT *************************/

OpenCLBayerFilterFloat::OpenCLBayerFilterFloat()
{
  kernel_name = "bayer";
  //output_element_size = sizeof(float) * 3;//3 channels
}


void OpenCLBayerFilterFloat::copyDataToGPU(const unsigned char* data_input, size_t di_size)
{
  cl_int err;

  err = clEnqueueWriteBuffer(command_queue, kparams,CL_TRUE,0, sizeof(kernel_params), kernel_params, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer: BayerFilterFloat  kernel params to GPU")
  
  err = clEnqueueWriteBuffer(command_queue, input,CL_TRUE,0, di_size, data_input, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer: BayerFilterFloat input image to GPU");
  
  err = clEnqueueWriteBuffer(command_queue, mem_balance,CL_TRUE,0, sizeof(balance), balance, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer: BayerFilterFloat balance to GPU");
}

void OpenCLBayerFilterFloat::setKernelArgs(size_t di_size, size_t do_size)
{
  releaseMem();

  cl_int err;
    
  kparams = clCreateBuffer(context,CL_MEM_READ_ONLY, sizeof(kernel_params),NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer kparams");
  input = clCreateBuffer(context,CL_MEM_READ_ONLY,di_size,NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer input");
  output = clCreateBuffer(context,CL_MEM_WRITE_ONLY,do_size,NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer output");
  output = clCreateBuffer(context,CL_MEM_WRITE_ONLY,sizeof(balance),NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer balance");

  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &input);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 0")
  
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &output);
  ASSERT_OPENCL_ERR(err,"Cant set kernel arg 1")

  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &kparams);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 2")

  err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*) &balance);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 3")
}


void OpenCLBayerFilterFloat::getResult (unsigned char* data_output, size_t do_size)
{
  cl_int err = clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, do_size, data_output, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err, "Cant enqueue read buffer")
}

/************************* IMAGE ************************************/

OpenCLBayerFilterImage::OpenCLBayerFilterImage(BayerFilterMask mask_type)
{
  switch(mask_type) {
    case BayerFilterMask::CROSS:
      kernel_name = "bayer_image_cross_mask";
      break;
    case BayerFilterMask::CIRCLE:
      kernel_name = "bayer_image_circle_mask";
      break;
    case BayerFilterMask::SQUARE:
    default:
      kernel_name = "bayer_image";
      break;
  }
  input_format.image_channel_order = CL_LUMINANCE;
  input_format.image_channel_data_type = CL_FLOAT;
  output_format.image_channel_order = CL_RGBA;
  output_format.image_channel_data_type= CL_FLOAT;
}

void OpenCLBayerFilterImage::copyDataToGPU(const unsigned char* data_input, size_t di_size)
{
  cl_int err;

  size_t origin[] = {0,0,0};
  size_t region[] = {params.width, params.height, 1};

  err = clEnqueueWriteBuffer(command_queue, kparams, CL_TRUE, 0, sizeof(kernel_params), kernel_params, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer: BayerFilterImage kernel params");
    
  err = clEnqueueWriteImage(command_queue, input, CL_TRUE, origin, region, 0, 0, (void*)data_input, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write image: BayerFilterImage");

  err = clEnqueueWriteBuffer(command_queue, mem_balance, CL_TRUE, 0, sizeof(balance), balance, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write image: BayerFilterImage");
}

void OpenCLBayerFilterImage::setKernelArgs (size_t di_size, size_t do_size)
{
  releaseMem();

  cl_int err;
  
  //create structures
  kparams = clCreateBuffer(context,CL_MEM_READ_ONLY, sizeof(kernel_params),NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer kparams");
    
  input = clCreateImage2D(context, CL_MEM_READ_ONLY, &input_format, params.width, params.height, 0, NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating image2D for input");

  output = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &output_format, params.width, params.height, 0, NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating image2D for output");

  /*sampler = clCreateSampler(context, CL_TRUE, CL_ADDRESS_MIRRORED_REPEAT, CL_FILTER_LINEAR, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating sampler");*/
 
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &input);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 0");
  
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &output);
  ASSERT_OPENCL_ERR(err,"Cant set kernel arg 1");

  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &kparams);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 2");

  err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*) &mem_balance);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 3");
}

void OpenCLBayerFilterImage::releaseMem()
{
  OpenCLBayerFilter::releaseMem();
  //clReleaseSampler(sampler);
}

void OpenCLBayerFilterImage::getResult (unsigned char* data_output, size_t do_size)
{
  size_t origin[] = {0,0,0};
  size_t region[] = {params.width, params.height, 1};
  cl_int err = clEnqueueReadImage(command_queue, output, CL_TRUE, origin, region, 0, 0, (void*)data_output, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err, "Cant enqueue read buffer. BayerFilterImage")
}

void OpenCLBayerFilterImage::copyDataToGPUStream()
{
  cl_int err;

  err = clEnqueueWriteBuffer(command_queue, kparams,CL_TRUE, 0, sizeof(kernel_params), kernel_params, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write buffer: BayerFilterImage for Stream kernel params");

  err = clEnqueueWriteBuffer(command_queue, mem_balance, CL_TRUE, 0, sizeof(balance), balance, 0, NULL, NULL);
  ASSERT_OPENCL_ERR(err,"Cant enqueue write mem_balance: BayerFilterImage");
}

void OpenCLBayerFilterImage::setKernelArgsForStream()
{
  //TODO: Release memory
  cl_int err;
  
  //create structures
  kparams = clCreateBuffer(context,CL_MEM_READ_ONLY, sizeof(kernel_params),NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer kparams");
  
  mem_balance = clCreateBuffer(context,CL_MEM_READ_ONLY, sizeof(balance),NULL, &err);
  ASSERT_OPENCL_ERR(err, "Error while creating buffer balance");

  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &kparams);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 2 - kparams for BayerFilterImage");

  err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*) &mem_balance);
  ASSERT_OPENCL_ERR(err, "Cant set kernel arg 2 - kparams for BayerFilterImage");
}