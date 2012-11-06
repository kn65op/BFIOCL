#ifndef OPENCLIMAGEFILTER_H
#define OPENCLIMAGEFILTER_H

#include <OpenCLAlgorithm.h>
#include <OpenCLBayerFilter.h>
#include <opencv2/core/core.hpp>

class OpenCLImageFilter
{
public:
  OpenCLImageFilter(std::string filename, int mode = BFIOCL_PAT_RGR);
  ~OpenCLImageFilter();
  
  cv::Mat getInputImage() const;
  cv::Mat getOutputImage();
  bool saveOutputImage(std::string filename);
  
private:
  void run();
  
  cv::Mat input_image;
  cv::Mat output_image;
  OpenCLBayerFilter algorithm;
  int mode;
  
  
};

#endif // OPENCLIMAGEFILTER_H
