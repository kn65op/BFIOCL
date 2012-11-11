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

class OpenCLBayerFilterParams : public OpenCLAlgorithmParams
{
public:
  OpenCLBayerFilterParams(unsigned int width = 0, unsigned int height = 0, cl_uchar pattern = BFIOCL_PAT_RGR, int mode = BFIOCL_MODE_BGR)
    : width(width), height(height), pattern(pattern), mode(mode) {}
  unsigned int width, height;
  int mode;
  cl_uchar pattern;
};

class OpenCLBayerFilter : public OpenCLAlgorithm
{

public:
  virtual void setParams(const OpenCLAlgorithmParams& params);
  void setParams(const OpenCLBayerFilterParams& params);
  virtual void prepare();
  virtual void run(const unsigned char* data_input, size_t di_size, unsigned char* data_output, size_t do_size);
  
private:
  static const std::string SOURCEFILE;
  OpenCLBayerFilterParams params;

  //work size
  size_t global_work_size[2];
};

#endif // OPENCLBAYERFILTER_H
