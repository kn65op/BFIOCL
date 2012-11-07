#include "OpenCLImageFilter.h"
#include <OpenCLBayerFilter.h>

#include <opencv2/highgui/highgui.hpp>
#include <iostream>

OpenCLImageFilter::OpenCLImageFilter(std::string filename, int mode)
{
  cv::Mat tmp;
  //convert image
  cv::imread(filename, -1).convertTo(tmp, CV_32F, 1.0/255.0f);

  //set input image
  setInputImage(tmp);

  algorithm = OpenCLBayerFilter();
  algorithm.setDevice(OpenCLDevice::getDevices().front());
  algorithm.prepare();
  this->mode = mode;
}

void OpenCLImageFilter::setInputImage(const cv::Mat & source)
{
  input_image = cv::Mat::zeros(source.rows + 2, source.cols + 2, source.type());

  //copy data
  int is, idest, js, jdest;
  for (is = 0, idest = 1; is < source.rows; ++is, ++idest)
  {
    for (js = 0, jdest = 1; js < source.cols; ++js, ++jdest)
    {
      input_image.at<float>(idest, jdest) = source.at<float>(is, js);
    }
  }
}

OpenCLImageFilter::~OpenCLImageFilter()
{
 
}

bool OpenCLImageFilter::saveOutputImage(std::string filename)
{
  cv::Mat tmp;
  getOutputImage().convertTo(tmp, CV_8U, 255);
  return cv::imwrite(filename, tmp);
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
    bool ok = input_image.isContinuous();
    uchar * input_data = input_image.data + (input_image.cols + 1) * sizeof(float);
    output_image.create(input_image.rows - 2, input_image.cols - 2, CV_32FC3);
    algorithm.setParams(OpenCLBayerFilterParams(output_image.cols, output_image.rows, mode, BFIOCL_MODE_BGR));
    algorithm.run(input_data, 
		  input_image.total()*input_image.elemSize() - (input_image.cols + 1) * sizeof(float),
		  output_image.data,
		  output_image.total()*output_image.elemSize()
		);
  }
  catch (OpenCLAlgorithmException ex)
  {
     std::cout << "OpenCLAlgorithmException: " << ex.getFullMessage() << "\n";
  }
}





