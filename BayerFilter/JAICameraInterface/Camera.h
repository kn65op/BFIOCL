#pragma once

#include<list>

#include <opencv2/core/core.hpp>

#include <Jai_Factory.h>

namespace JAI
{

class Camera
{
public:
  Camera(int8_t index);
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
  FACTORY_HANDLE  m_hFactory;     // Factory Handle
  CAM_HANDLE      m_hCam;         // Camera Handle
  THRD_HANDLE     m_hThread;      // Acquisition Thread Handle
  int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID
  IplImage        *m_pImg;        // OpenCV Images
  IplImage        *m_pOutputImg;
  IplImage        *m_pColorOutputImg;
  CvMat           *m_pMapMatrix;  // OpenCV Matrix

  //max size of queue
  static const unsigned int queue_size = 100;

  //JAI specific objects

};

}
