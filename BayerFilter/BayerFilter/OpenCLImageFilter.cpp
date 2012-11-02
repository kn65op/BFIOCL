#include "OpenCLImageFilter.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

OpenCLImageFilter::OpenCLImageFilter(std::string filename)
{
    cv::imread(filename, -1).convertTo(input_image, CV_32F, 1.0/255.0f);
    algorithm = OpenCLInvertImage();
    algorithm.setDevice(OpenCLDevice::getDevices().front());  
    
//     std::cout << "Depth: " << (input_image.depth() == CV_8U ? std::string("CV_8U") : std::string("not")) << "\n";
//     std::cout << "Total: " << input_image.total() << "\n";
//     std::cout << "ElemSize: " << input_image.elemSize() << "\n";
//     std::cout << "Channels: " << input_image.channels() << "\n";
//     std::cout << "data(5,6): " << (float) input_image.at<float>(5,6) << "\n";
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
  if (output_image.empty()) {
    run();
  }
  return output_image;  
}

void OpenCLImageFilter::run()
{
  try
  {
    std::cout << "Prepare...\n";
    algorithm.prepare();
    std::cout << "output image creating...\n";
    output_image.create(input_image.rows, input_image.cols, CV_32F);
    std::cout << "Algorithm run...\n";
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





