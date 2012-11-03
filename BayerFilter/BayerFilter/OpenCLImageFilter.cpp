#include "OpenCLImageFilter.h"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>

OpenCLImageFilter::OpenCLImageFilter(std::string filename)
{
    cv::imread(filename, -1).convertTo(input_image, CV_32F, 1.0/255.0f);
    algorithm = OpenCLInvertImage();
    algorithm.setDevice(OpenCLDevice::getDevices().front());
}

OpenCLImageFilter::~OpenCLImageFilter()
{
 
}


cv::Mat OpenCLImageFilter::getInputImage() const
{
  return input_image;
}

cv::Mat OpenCLImageFilter::getOutputImage()
{
  if (output_image.empty()) run();
  return output_image;  
}

void OpenCLImageFilter::run()
{
  try
  {
    output_image.create(input_image.rows, input_image.cols, CV_32F);
    algorithm.setParams(OpenCLInvertImageParams(output_image.cols, output_image.rows));
    algorithm.prepare();
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





