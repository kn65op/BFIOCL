#ifndef OPENCLIMAGEFILTER_H
#define OPENCLIMAGEFILTER_H

#include <OpenCLAlgorithm.h>
#include <OpenCLBayerFilter.h>
#include <opencv2/core/core.hpp>

class OpenCLImageFilter
{
public:
  OpenCLImageFilter(std::string filename, cl_uchar mode = BFIOCL_PAT_RGR);
  ~OpenCLImageFilter();
  
  cv::Mat getInputImage() const;
  cv::Mat getOutputImage();
  cv::Mat getOutputImageOpenCV();

  
  bool saveOutputImage(std::string filename);
  
private:
  void run();
  void setInputImage(const cv::Mat & source);
  
  cv::Mat input_image_raw;
  cv::Mat input_image;
  cv::Mat output_image;
  cv::Mat output_image_open_cv;
  OpenCLBayerFilter algorithm;
  cl_uchar mode;
  
  
};

#endif // OPENCLIMAGEFILTER_H

