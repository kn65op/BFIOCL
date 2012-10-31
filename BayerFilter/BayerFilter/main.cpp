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
      devs.front().getCommandQueue();
    }
  }
  catch (OpenCLDeviceException e)
  {
    std::cout << e.getFullMessage() << "\n";
  }
  
  return 0;
}

