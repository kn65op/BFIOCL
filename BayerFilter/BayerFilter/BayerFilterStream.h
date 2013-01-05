#pragma once

#include <OpenCLAlgorithmsStream.h>

#include <string>

#include <opencv2\core\core.hpp>

class BayerFilterStream
{
public:
  BayerFilterStream(int w, int h, cl_uchar mode, float red_k = 1.0, float green_k = 1.0, float blue_k = 1.0);
  ~BayerFilterStream(void);

  void setFiles(std::string in, std::string out);
  void processImage(cv::Mat & source, cv::Mat & dest);
private:
  OpenCLAlgorithmsStream stream;
};

