#pragma once

#include <OpenCLAlgorithmsStream.h>

#include <string>

class BayerFilterStream
{
public:
  BayerFilterStream(int w, int h, cl_uchar mode);
  ~BayerFilterStream(void);

  void setFiles(std::string in, std::string out);
private:
  OpenCLAlgorithmsStream stream;
};

