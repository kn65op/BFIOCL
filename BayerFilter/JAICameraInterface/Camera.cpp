#include "Camera.h"
#include "CameraException.h"

#include <iostream>

using namespace JAI;

#define NODE_NAME_WIDTH         "Width"
#define NODE_NAME_HEIGHT        "Height"
#define NODE_NAME_PIXELFORMAT   "PixelFormat"
#define NODE_NAME_GAIN          "GainRaw"
#define NODE_NAME_ACQSTART      "AcquisitionStart"
#define NODE_NAME_ACQSTOP       "AcquisitionStop"

FACTORY_HANDLE Camera::m_hFactory;

Camera::Camera(int8_t* index)
{
  m_hFactory = NULL;
  m_hCam = NULL;
  m_hThread = NULL;
}


Camera::~Camera(void)
{
  close();
}

void Camera::close()
{
  if (m_hCam)
    {
        // Close camera
        J_Camera_Close(m_hCam);
        m_hCam = NULL;
    }

    if (m_hFactory)
    {
        // Close factory
        J_Factory_Close(m_hFactory);
        m_hFactory = NULL;
    }
}

bool Camera::open()
{
  J_STATUS_TYPE   retval;

  // Open camera
  retval = J_Camera_Open(m_hFactory, m_sCameraId, &m_hCam);
  if (retval != J_ST_SUCCESS)
  {
    //TODO:throw error
    //AfxMessageBox(CString("Could not open the camera!"), MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }
  std::cout << "Opening camera succeeded\n";

  return TRUE;
}

std::list<Camera*> Camera::getCameraList()
{
  std::list<Camera*> ret; //TODO: autoptr
  J_STATUS_TYPE   retval;
  uint32_t        iSize;
  uint32_t        iNumDev;
  bool8_t         bHasChange;
  
  // Open factory
  retval = J_Factory_Open("" , &m_hFactory);
  if (retval != J_ST_SUCCESS)
  {
    throw CameraException("Could not open factory!");
    return ret;
  }
  std::cout << "Opening factory succeeded\n";

  // Update camera list
  retval = J_Factory_UpdateCameraList(m_hFactory, &bHasChange);
  if (retval != J_ST_SUCCESS)
  {
    throw CameraException("Could not update camera list!");
  }
  std::cout << "Updating camera list succeeded\n";

  // Get the number of Cameras
  retval = J_Factory_GetNumOfCameras(m_hFactory, &iNumDev);
  if (retval != J_ST_SUCCESS)
  {
    throw CameraException("Could not get the number of cameras!");
  }
  if (iNumDev == 0)
  {
    throw CameraException("There is no camera!");
  }
  std::cout << "%d cameras were found" << iNumDev << "\n";

  // Get camera ID
  int8_t m_sCameraId[J_CAMERA_ID_SIZE];
  for (uint32_t i=0; i<iNumDev; ++i)
  {
    iSize = (uint32_t)sizeof(m_sCameraId);
    retval = J_Factory_GetCameraIDByIndex(m_hFactory, i, m_sCameraId, &iSize);
    ret.push_back(new Camera(m_sCameraId));
    //if (retval != J_ST_SUCCESS)
    //{
      //TODO: throw?
      //AfxMessageBox(CString("Could not get the camera ID!"), MB_OK | MB_ICONEXCLAMATION);
      //return ret;

    //}
  }
  std::cout << "Camera ID: %s\n" << m_sCameraId << "\n";
  return ret;
}

bool Camera::start()
{
  J_STATUS_TYPE   retval;
  int64_t int64Val;
  int64_t pixelFormat;

  SIZE	ViewSize;
  POINT	TopLeft;

  // Get Width from the camera
  retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_WIDTH, &int64Val);
  ViewSize.cx = (LONG)int64Val;     // Set window size cx

  // Get Height from the camera
  retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);
  ViewSize.cy = (LONG)int64Val;     // Set window size cy

  // Get pixelformat from the camera
  retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &int64Val);
  pixelFormat = int64Val;

  // Calculate number of bits (not bytes) per pixel using macro
  int bpp = J_BitsPerPixel(pixelFormat);

  // Set window position
  TopLeft.x = 100;
  TopLeft.y = 50;

  // Open stream
  retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&Camera::callback), &m_hThread, (ViewSize.cx*ViewSize.cy*bpp)/8);
  if (retval != J_ST_SUCCESS) {
    return false;
  }
  std::cout << "Opening stream succeeded\n";

  // Start Acquision
  retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);

  return true;
}

void Camera::stop()
{
  J_STATUS_TYPE retval;

  // Stop Acquision
  if (m_hCam) {
    retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTOP);
  }

  if(m_hThread)
  {
    // Close stream
    retval = J_Image_CloseStream(m_hThread);
    m_hThread = NULL;
    std::cout << "Closed stream\n";
  }
}

void Camera::callback(J_tIMAGE_INFO * pAqImageInfo)
{
  if (queue.size() == queue_size) //max limit for queue 
  {
    return;
  }
  cv::Mat * tmp;
  if (free_queue.empty()) //get new image
  {
    tmp = new cv::Mat(pAqImageInfo->iSizeX, pAqImageInfo->iSizeY, CV_8UC1);
  }
  else //get image from free_queue
  {
    tmp = free_queue.front();
    free_queue.pop();
  }
  memcpy(tmp->data, pAqImageInfo->pImageBuffer, tmp->total() * tmp->elemSize());
  queue.push(tmp);
}

cv::Mat Camera::getNextFrame()
{
  if (queue.empty())
  {
    throw NoNewFrameException();
  }
  cv::Mat * ret = queue.front();
  queue.pop();
  free_queue.push(ret);
  return *ret;
}