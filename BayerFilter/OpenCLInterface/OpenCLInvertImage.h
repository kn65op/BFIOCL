#ifndef OPENCLINVERTIMAGE_H
#define OPENCLINVERTIMAGE_H

#include "OpenCLAlgorithm.h"

class OpenCLInvertImageParams: public OpenCLAlgorithmParams {
public:
  OpenCLInvertImageParams(unsigned int w = 0, unsigned int h = 0): width(w), height(h) {}
  virtual ~OpenCLInvertImageParams() {};
  unsigned int width;
  unsigned int height;
};

class OpenCLInvertImage : public OpenCLAlgorithm
{

public:
  OpenCLInvertImage(void);
  ~OpenCLInvertImage();
virtual void run(const unsigned char* data_input, size_t di_size, unsigned char* data_output, size_t do_size);
virtual void prepare();
virtual void setParams(const OpenCLAlgorithmParams& params);
virtual void setParams(const OpenCLInvertImageParams& params);
private:
  
  
  static const std::string SOURCE;
  OpenCLInvertImageParams params;
};

#endif // OPENCLINVERTIMAGE_H
