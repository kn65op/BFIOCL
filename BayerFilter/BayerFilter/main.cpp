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
    std::cout << "Devices found: " << devs.size() << "\n";
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
  if (argv < 3) {
    std::cout << "Usage: " << argc[0] << " 0-3 filename.bmp [output_filename.bmp]\n";
    std::cout << "\n\t0-3 - bayer matix start mode\n";
    std::cout << "\twith specified output file dont show images, only save new file\n";
    return 1;
  }
  
  //std::cout << "Devices list:\n";
  //list_devices();
  
  try
  {
    uchar mode = (uchar)atoi(argc[1]);
    OpenCLImageFilter filter(argc[2], mode);
    if (argv > 3) {
      filter.saveOutputImage(argc[3]);
      std::cout << "Image '" << argc[3] << "' saved!\n";
      if (argv > 5)
      {
        filter.setInputImage(argc[4]);
        filter.saveOutputImage(argc[5]);
      std::cout << "Image '" << argc[5] << "' saved!\n";
      }
    }
    else
    {
      cv::imshow("Input Image", filter.getInputImage());
      cv::imshow("Output Image", filter.getOutputImage());
      cv::imshow("Output Image original", filter.getOutputImageOpenCV());
      //cv::imwrite("tmp_OpenCV.bmp", filter.getOutputImageOpenCV());
      
      while( cv::waitKey(10) < 0 ) {;}
    }

  }
  catch (OpenCLDeviceException e)
  {
    std::cout << e.getFullMessage() << "\n";
  }
  
  
  return 0;
}

