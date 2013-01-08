#pragma once

#include <OpenCLAlgorithmsStream.h>

#include <string>

#include <opencv2\core\core.hpp>

/**
 * Class which performs Bayer filter with coversions from int to float on input and from float to int on output.
 */
class BayerFilterStream
{
public:
  /**
   * Constructor with setting main parameters of Bayer Filter.
   * @param w Image width.
   * @param h Image height.
   * @param mode Bayer filter mode. Can be 0-3 and represents type of Bayer Filter.
   * @param red_k Red color factor. Can be from 0 to 1. Default is 1.
   * @param green_k Green color factor. Can be from 0 to 1. Default is 1.
   * @param blue_k Blue color factor. Can be from 0 to 1. Default is 1.
   */
  BayerFilterStream(int w, int h, cl_uchar mode, float red_k = 1.0, float green_k = 1.0, float blue_k = 1.0);
  ~BayerFilterStream(void);

  /**
   * Set source and destination files and perform interpolation.
   * @param in Source file.
   * @param out Destination file.
   */
  void setFiles(std::string in, std::string out);
  
  /**
   * Set source and destination image. Images are in cv::Mat format.
   * @param source Source image.
   * @param dest Destination image. Important, image must be allocated before for reasons of efficiency.
   */
  void processImage(cv::Mat & source, cv::Mat & dest);

  /**
   * Get all time consumed.
   * @return Time consumed on all execution of stream.
   */
  double getAllTime();
private:
  OpenCLAlgorithmsStream stream;

  double all_time;
};

