#pragma once

#include<list>

#include <opencv2/core/core.hpp>

#include <Jai_Factory.h>

namespace JAI
{

class Camera
{
public:
  Camera(int index);
  ~Camera(void);

  //open/close
  bool open();
  bool close();

  bool start();
  bool stop();

  cv::Mat & getNextFrame();

  //void setCallbackFunction(

  static std::list<Camera*> getCameraList();

private:
  int index;
  unsigned char * id[J_CAMERA_ID_SIZE];

  //max size of queue
  static const unsigned int queue_size = 100;
};

}
