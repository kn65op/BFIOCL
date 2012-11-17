#include "OpenCLImageFilter.h"
#include <OpenCLBayerFilter.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

OpenCLImageFilter::OpenCLImageFilter(std::string filename, cl_uchar mode)
{
  //convert image
  cv::Mat tmp;
  input_image_raw = cv::imread(filename, -1);
  input_image_raw.convertTo(tmp, CV_32F, 1.0/255.0f);

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
  output_image.create(input_image.rows - 2, input_image.cols - 2, CV_32FC3);
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
  run();
  //cv::Vec3f t = output_image.at<cv::Vec3f>(100, 30);
  //std::cout << "Pixel(100,30): " << (float) t[0] << ", " << (float) t[1] << ", " << (float) t[2] << "\n";
  std::cout << algorithm.getTimeConsumed() << "\n";
  return output_image;  
}

cv::Mat OpenCLImageFilter::getOutputImageOpenCV()
{
  if (output_image_open_cv.empty())
  {
    cv::cvtColor(input_image_raw, output_image_open_cv, CV_BayerRG2BGR);
  }
  return output_image_open_cv;  
}

void OpenCLImageFilter::run()
{
  if (!input_image.isContinuous())
  {
    throw OpenCLAlgorithmException("Data is not contunuous");
  }
  try
  {
    uchar * input_data = input_image.data;
    algorithm.setParams(OpenCLBayerFilterParams(output_image.cols, output_image.rows, mode, BFIOCL_MODE_BGR));
    algorithm.run(input_data, 
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





