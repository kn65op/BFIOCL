#ifndef OPENCLIMAGEFILTER_H
#define OPENCLIMAGEFILTER_H

#include <OpenCLAlgorithm.h>
#include <OpenCLBayerFilter.h>
#include <opencv2/core/core.hpp>

class OpenCLImageFilter
{
public:
  OpenCLImageFilter(std::string filename);
  ~OpenCLImageFilter();
  
  cv::Mat getInputImage() const;
  cv::Mat getOutputImage();
  
private:
  void run();
  
  cv::Mat input_image;
  cv::Mat output_image;
  OpenCLBayerFilter algorithm;
  
  
};

#endif // OPENCLIMAGEFILTER_H
