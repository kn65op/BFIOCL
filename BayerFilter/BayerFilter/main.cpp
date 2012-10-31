#include <OpenCLDevice.h>
#include <iostream>
#include <list>

int main(int argv, char * argc[])
{
  try
  {
    std::list<OpenCLDevice> devs = OpenCLDevice::getDevices();
    if (! devs.empty() )
    {
      std::cout << "Platform: " << devs.front().getPlatformName()<< "\n";
      std::cout << "Device: " << devs.front().getName() << "\n";
      devs.front().getContext();
    }
  }
  catch (CLDeviceException e)
  {
    std::cout << e.message << "\n";
  }
  
  return 0;
}

