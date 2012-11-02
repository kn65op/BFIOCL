#ifndef OPENCLINVERTIMAGE_H
#define OPENCLINVERTIMAGE_H

#include "OpenCLAlgorithm.h"


class OpenCLInvertImage : public OpenCLAlgorithm
{

public:
  
  ~OpenCLInvertImage();
virtual void run(const unsigned char* data_input, size_t di_size, unsigned char* data_output, size_t do_size);
virtual void prepare();
virtual void setParams(const OpenCLAlgorithmParams& params);

private:
  static const std::string SOURCE;
};

#endif // OPENCLINVERTIMAGE_H
