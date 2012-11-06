#include "OpenCLImageFilter.h"
#include <OpenCLBayerFilter.h>

#include <opencv2/highgui/highgui.hpp>
#include <iostream>

OpenCLImageFilter::OpenCLImageFilter(std::string filename, int mode)
{
    cv::imread(filename, -1).convertTo(input_image, CV_32F, 1.0/255.0f);
    algorithm = OpenCLBayerFilter();
    algorithm.setDevice(OpenCLDevice::getDevices().front());
    algorithm.prepare();
    this->mode = mode;
}

OpenCLImageFilter::~OpenCLImageFilter()
{
 
}

bool OpenCLImageFilter::saveOutputImage(std::string filename)
{
  cv::Mat tmp;
  getOutputImage().convertTo(tmp, CV_8U, 255);
  cv::imwrite(filename, tmp);
}



cv::Mat OpenCLImageFilter::getInputImage() const
{
  return input_image;
}

cv::Mat OpenCLImageFilter::getOutputImage()
{
  if (output_image.empty()) run();
  //cv::Vec3f t = output_image.at<cv::Vec3f>(100, 30);
  //std::cout << "Pixel(100,30): " << (float) t[0] << ", " << (float) t[1] << ", " << (float) t[2] << "\n";
  return output_image;  
}

void OpenCLImageFilter::run()
{
  try
  {
    output_image.create(input_image.rows, input_image.cols, CV_32FC3);
    algorithm.setParams(OpenCLBayerFilterParams(output_image.cols, output_image.rows, mode, BFIOCL_MODE_BGR));
    algorithm.run(input_image.data, 
		  input_image.total()*input_image.elemSize(),
		  output_image.data,
		  output_image.total()*output_image.elemSize()
		);
  }
  catch (OpenCLAlgorithmException ex)
  {
     std::cout << "OpenCLAlgorithmException: " << ex.getFullMessage() << "\n";
  }
}





