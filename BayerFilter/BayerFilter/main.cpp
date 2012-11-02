#include <OpenCLDevice.h>
#include "OpenCLImageFilter.h"
#include <iostream>
#include <list>
#include <opencv2/highgui/highgui.hpp>

void list_devices() 
{
  try
  {
    std::list<OpenCLDevice> devs = OpenCLDevice::getDevices();
    if (! devs.empty() )
    {
      std::cout << "Platform: " << devs.front().getPlatformName()<< "\n";
      std::cout << "Device: " << devs.front().getName() << "\n";
      devs.front().getCommandQueue();
    }
  }
  catch (OpenCLDeviceException e)
  {
    std::cout << e.getFullMessage() << "\n";
  }
}


int main(int argv, char * argc[])
{
  if (argv < 2) {
    std::cout << "Usage: " << argc[0] << " filename.bmp\n";
    return -1;
  }
  
  OpenCLImageFilter filter(argc[1]);
  cv::imshow("Input Image", filter.getInputImage());
  cv::imshow("Output Image", filter.getOutputImage());

  
  while( cv::waitKey(10) < 0 ) {;}
  
  
  return 0;
}

