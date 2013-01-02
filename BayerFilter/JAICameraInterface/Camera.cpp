#include "Camera.h"

#include <iostream>

using namespace JAI;

Camera::Camera(int8_t* index)
{
  m_hFactory = NULL;
  m_hCam = NULL;
  m_hThread = NULL;
  m_pImg = NULL;
}


Camera::~Camera(void)
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
    //TODO: throw
    //AfxMessageBox(CString("Could not open factory!"));
    return ret;
  }
  std::cout << "Opening factory succeeded\n";

  // Update camera list
  retval = J_Factory_UpdateCameraList(m_hFactory, &bHasChange);
  if (retval != J_ST_SUCCESS)
  {
    //TODO: throw
    //AfxMessageBox(CString("Could not update camera list!"), MB_OK | MB_ICONEXCLAMATION);
    return ret;
  }
  std::cout << "Updating camera list succeeded\n";

  // Get the number of Cameras
  retval = J_Factory_GetNumOfCameras(m_hFactory, &iNumDev);
  if (retval != J_ST_SUCCESS)
  {
    //TODO: throw
    //AfxMessageBox(CString("Could not get the number of cameras!"), MB_OK | MB_ICONEXCLAMATION);
    return ret;
  }
  if (iNumDev == 0)
  {
    //TODO: throw
    //AfxMessageBox(CString("There is no camera!"), MB_OK | MB_ICONEXCLAMATION);
    return ret;
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
