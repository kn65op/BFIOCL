#include "Camera.h"

using namespace JAI;

Camera::Camera(int8_t index)
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
