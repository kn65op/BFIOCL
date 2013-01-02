#pragma once

#include<list>
#include<queue>

#include <opencv2/core/core.hpp>

#include <Jai_Factory.h>

namespace JAI
{

class Camera
{
public:
  Camera(int8_t* index);
  ~Camera(void);

  //open/close
  bool open();
  void close();

  bool start();
  void stop();

  cv::Mat & getNextFrame();

  //void setCallbackFunction(

  static std::list<Camera*> getCameraList();

private:
  int index;

  //JAI specific objects
  static FACTORY_HANDLE  m_hFactory;     // Factory Handle
  CAM_HANDLE      m_hCam;         // Camera Handle
  THRD_HANDLE     m_hThread;      // Acquisition Thread Handle
  int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID
  CvMat           *m_pMapMatrix;  // OpenCV Matrix

  //max size of queue
  static const unsigned int queue_size = 100;

  std::queue<cv::Mat*> queue;
  std::queue<cv::Mat*> free_queue;

  void callback(J_tIMAGE_INFO * pAqImageInf);


};

}
