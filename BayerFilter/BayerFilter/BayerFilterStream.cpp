#include "BayerFilterStream.h"
#include <OpenCLBayerFilter.h>
#include <OpenCLDevice.h>
#include <OpenCLIntToFloat.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

BayerFilterStream::BayerFilterStream(int w, int h, cl_uchar mode)
{
  stream.setDataSize(w,h);
  stream.setDevice(OpenCLDevice::getDevices().front());

  OpenCLImageAlgorithm* bayer = new OpenCLBayerFilterImage();
  bayer->setParams(OpenCLBayerFilterParams(w, h, mode, BFIOCL_MODE_BGR));

  stream.pushAlgorithm(new OpenCLIntToFloat());
  stream.pushAlgorithm(bayer);
  stream.prepare();
}


BayerFilterStream::~BayerFilterStream(void)
{
  stream.clearAlgorithms();
}


void BayerFilterStream::setFiles(std::string in, std::string out)
{
  cv::Mat input_image_raw = cv::imread(in, -1);
  cv::Mat tmp;
  cv::Mat output_image_raw(input_image_raw.size(), CV_32FC4);
  stream.processImage(input_image_raw.data, output_image_raw.data);
  output_image_raw.convertTo(tmp, CV_8U, 255);
  cv::imwrite(out, tmp);
}