#include "BayerFilterStream.h"
#include <OpenCLBayerFilter.h>
#include <OpenCLDevice.h>
#include <OpenCLIntToFloat.h>
#include <OpenCLFloatToInt.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

BayerFilterStream::BayerFilterStream(int w, int h, cl_uchar mode, float red_k, float green_k, float blue_k) 
{
  stream.setDataSize(w,h);
  stream.setDevice(OpenCLDevice::getDevices().front());

  OpenCLImageAlgorithm* bayer = new OpenCLBayerFilterImage();
  bayer->setParams(OpenCLBayerFilterParams(w, h, mode, BFIOCL_MODE_BGR, red_k, green_k, blue_k));

  stream.pushAlgorithm(new OpenCLIntToFloat());
  stream.pushAlgorithm(bayer);
  stream.pushAlgorithm(new OpenCLFloatToInt(OpenCLFloatToIntMode::UINT16));
  stream.prepare();
}


BayerFilterStream::~BayerFilterStream(void)
{
  stream.clearAlgorithms();
}


void BayerFilterStream::setFiles(std::string in, std::string out)
{
  cv::Mat input_image_raw = cv::imread(in, -1);
  if (!input_image_raw.isContinuous())
  {
    throw OpenCLException("Not continuous", 0);
  }
  cv::Mat output_image_raw(input_image_raw.size(), CV_16UC4);
  stream.processImage(input_image_raw.data, output_image_raw.data);
  cv::imwrite(out, output_image_raw);
  std::cout << stream.getTime() << "\n";
}

void BayerFilterStream::processImage(cv::Mat & sc, cv::Mat & dst)
{
  dst = cv::Mat(sc.size(), CV_8UC4);
  stream.processImage(sc, dst);
}