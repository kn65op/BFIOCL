#include <OpenCLDevice.h>
#include <iostream>
#include <list>

int main(int argv, char * argc[])
{
  std::list<OpenCLDevice> devs = OpenCLDevice::getDevices();
  if (! devs.empty() )
  {
    std::cout << "Platform: " << devs.front().getPlatformName()<< "\n";
    std::cout << "Device: " << devs.front().getName() << "\n";
    devs.front().getContext();
  }
  
  return 0;
}

