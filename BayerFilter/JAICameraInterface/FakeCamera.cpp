#include "FakeCamera.h"

#include <iostream>
#include <sstream>

#include <opencv/highgui.h>

using namespace JAI;

#define NODE_NAME_WIDTH         "Width"
#define NODE_NAME_HEIGHT        "Height"
#define NODE_NAME_PIXELFORMAT   "PixelFormat"
#define NODE_NAME_GAIN          "GainRaw"
#define NODE_NAME_ACQSTART      "AcquisitionStart"
#define NODE_NAME_ACQSTOP       "AcquisitionStop"

FakeCamera::FakeCamera(int8_t* index)
{
}


FakeCamera::~FakeCamera(void)
{
}

void FakeCamera::close()
{
}

bool FakeCamera::open()
{
  return true;
}

std::list<FakeCamera*> FakeCamera::getCameraList()
{
  std::list<FakeCamera*> ret;
  ret.push_back(new FakeCamera(NULL));
  return ret;
}

bool FakeCamera::start()
{
  return true;
}

void FakeCamera::stop()
{
}

cv::Mat FakeCamera::getNextFrame()
{
  static std::string name_prefix = "RecordedImage_BB-500GE_00-0C-DF-04-11-C3_0";
  static std::string name_postfix = ".bmp";
  static int d = 0;
  static int u = 0;
  static std::string dir = "../../data2/";
  std::stringstream filename;
  filename << dir << name_prefix << d << u << name_postfix;
  if (++u == 10)
  {
    u = 0;
    ++d;
  }
  cv::Mat ret = cv::imread(filename.str(), -1);
  return ret;
}