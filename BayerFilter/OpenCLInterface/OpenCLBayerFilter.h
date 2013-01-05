#ifndef OPENCLBAYERFILTER_H
#define OPENCLBAYERFILTER_H

//Input pattern first 3 pixels in first row
#define BFIOCL_PAT_RGR 0
#define BFIOCL_PAT_GRG 1
#define BFIOCL_PAT_GBG 2
#define BFIOCL_PAT_BGB 3

//Output modes, RGB, BGR, BRG, RBG, GRB, GBR
//In binary mode 00RRGGBB, where RR, BB, GG is color offset (0,1,2)
#define BFIOCL_MODE_RGB 0x06
#define BFIOCL_MODE_RBG 0x09
#define BFIOCL_MODE_BRG 0x12
#define BFIOCL_MODE_BGR 0x24
#define BFIOCL_MODE_GBR 0x21
#define BFIOCL_MODE_GRB 0x18

#include "OpenCLAlgorithm.h"

enum class BayerFilterMask
{
  SQUARE,
  CIRCLE,
  CROSS 
};

class OpenCLBayerFilterParams : public OpenCLAlgorithmParams
{
public:
  OpenCLBayerFilterParams(unsigned int width = 0, unsigned int height = 0, cl_uchar pattern = BFIOCL_PAT_RGR, int mode = BFIOCL_MODE_BGR, float red_b = 1.0, float green_b = 1.0, float blue_b = 1.0)
    : width(width), height(height), mode(mode), pattern(pattern)
  {
    balance[0] = red_b;
    balance[1] = green_b;
    balance[2] = blue_b;
  }

  unsigned int width, height;
  int mode;
  cl_uchar pattern;
  float balance[3];
};

/**
 * @brief Bayer filter.
 * 
 * It use float as parameter to OpenCL kernel.
 */
class OpenCLBayerFilter : virtual public OpenCLAlgorithm
{

public:
  OpenCLBayerFilter();
  ~OpenCLBayerFilter();
  virtual void setParams(const OpenCLAlgorithmParams& params);
  void setParams(const OpenCLBayerFilterParams& params);
  virtual void run(const unsigned char* data_input, size_t di_size, unsigned char* data_output, size_t do_size);
  
protected:
  static const std::string SOURCEFILE;
  OpenCLBayerFilterParams params;

  //function that differ for methods, 
  virtual void getResult(unsigned char* data_output, size_t do_size) = 0;
  virtual void releaseMem();
  virtual void copyDataToGPU(const unsigned char* data_input, size_t di_size) = 0;

  //for stream
  void setKernelArgsForStream();

  //work size
  size_t global_work_size[2];
  
  //kernel params
  static const unsigned int kernel_params_size = 4;
  cl_uchar kernel_params[kernel_params_size];

  //cl_mems
  cl_mem kparams, mem_balance;

  //color balance
  float balance[3];
};

class OpenCLBayerFilterFloat : public OpenCLBayerFilter
{
public:
  OpenCLBayerFilterFloat();

private:
  void setKernelArgs(size_t di_size, size_t do_size);
  void getResult(unsigned char* data_output, size_t do_size);
  void copyDataToGPU(const unsigned char* data_input, size_t di_size);
};

class OpenCLBayerFilterImage : virtual public OpenCLBayerFilter, public OpenCLImageAlgorithm
{
public:
  OpenCLBayerFilterImage(BayerFilterMask mask_type = BayerFilterMask::SQUARE);

private:
  void setKernelArgs(size_t di_size, size_t do_size);
  void getResult(unsigned char* data_output, size_t do_size);
  void copyDataToGPU(const unsigned char* data_input, size_t di_size);
  void releaseMem();
  void copyDataToGPUStream();
  void setKernelArgsForStream();

#ifndef __unix__
  #pragma warning (disable :4250) //for disable warning in vs about inhertance via dominance, may not work in gcc
#endif
};


#endif // OPENCLBAYERFILTER_H
