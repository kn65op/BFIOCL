#include "OpenCLImageFilter.h"
#include <OpenCLBayerFilter.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

OpenCLImageFilter::OpenCLImageFilter(std::string filename, cl_uchar mode)
{
  algorithm = new OpenCLBayerFilterImage();
  setInputImage(filename);

  algorithm->setDevice(OpenCLDevice::getDevices().front());
  algorithm->setParams(OpenCLBayerFilterParams(output_image.cols, output_image.rows, mode, BFIOCL_MODE_BGR));
  algorithm->prepare(input_image.total()*input_image.elemSize(), output_image.total()*output_image.elemSize());
  this->mode = mode;
}

void OpenCLImageFilter::setInputImage (const std::string filename)
{
  //convert image
  cv::Mat tmp;
  input_image_raw = cv::imread(filename, -1);
  input_image_raw.convertTo(tmp, CV_32F, 1.0/255.0f);

  //set input image
  setInputImage(tmp);
}

void OpenCLImageFilter::setInputImage(const cv::Mat & source)
{
  if (dynamic_cast<OpenCLBayerFilterImage*>(algorithm))
  {
    input_image = source;
    if (output_image.rows != input_image.rows && output_image.cols != input_image.cols)
    {
      output_image.create(input_image.rows, input_image.cols, CV_32FC4);
    }
    return;
  }
  if (input_image.cols + 2 != source.cols && input_image.rows + 2 != source.cols)
  {
    input_image = cv::Mat::zeros(source.rows + 2, source.cols + 2, source.type());
  }

  //copy data
  int is, idest, js, jdest;
  for (is = 0, idest = 1; is < source.rows; ++is, ++idest)
  {
    for (js = 0, jdest = 1; js < source.cols; ++js, ++jdest)
    {
      input_image.at<float>(idest, jdest) = source.at<float>(is, js);
    }
  }

  if (output_image.rows != input_image.rows - 2 && output_image.cols != input_image.cols - 2)
  {
    output_image.create(input_image.rows - 2, input_image.cols - 2, CV_32FC3);
  }
}

OpenCLImageFilter::~OpenCLImageFilter()
{
  delete algorithm;
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
  cv::Vec4f t = output_image.at<cv::Vec4f>(100, 30);
  std::cout << "Pixel(100,30): " << (float) t[0] << ", " << (float) t[1] << ", " << (float) t[2] << ", " << (float) t[3] << "\n";
  std::cout << algorithm->getTimeConsumed() << "\n";
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
    algorithm->run(input_data, 
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





