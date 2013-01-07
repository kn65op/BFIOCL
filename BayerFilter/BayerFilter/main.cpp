#include <iostream>
#include <chrono>

#include <OpenCLDevice.h>
#include <OpenCLHelpers.h>
#include "OpenCLImageFilter.h"
#include "BayerFilterStream.h"
#include "Options.h"
#include <FakeCamera.h>
#include <CameraException.h>
#include <Camera.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>



void printHelp(std::string program_name)
{
  std::cout << "Usage: " << program_name << " [mode] [parameters] [options] \n" << 
    "Modes: \n" <<
    "\t-c - read from JAI camera. Default mode. No options.\n" << 
    "\t-d DIR A- read files from dir. Files must have names Axxx.bmp, where A stand for common prefix and xxx for three positions number of image, wchich starts from 0. No options.\n" << 
    "\t-f FILE - read and process one file. Options: \n" <<
    "\t\t-o - ouptup file (optional)\n" <<
    "\t-h - print this help\n";
}

using std::chrono::milliseconds;
using std::chrono::duration_cast;
milliseconds ms;

int main (int argc, char * argv[])
{
  Options options;
  std::chrono::system_clock::time_point t0, tend;
  JAI::Camera * real_cam = nullptr;
  JAI::FakeCamera * fake_cam = nullptr;
  int x, y;
  x = 2456;
  y = 2058;
  BayerFilterStream bfs(x, y, 0, 0.8f, 0.7f, 0.9f);
  cv::Mat resize;
  cv::Mat image(y, x, CV_8UC4);

  switch (options.parseOptions(argc, argv))
  {
  case Mode::CAMERA:
    real_cam = JAI::Camera::getCameraList().front();
    real_cam->getImageSize(x, y);
    t0 = std::chrono::high_resolution_clock::now();
    if (real_cam->open() && real_cam->start())
    {
      for (int i=0; i < 100; ++i)
      {
        try
        {
          bfs.processImage(real_cam->getNextFrame(), image);
          cv::resize(image, resize, cv::Size(800, 600));
          imshow("s", resize);
          cv::waitKey(1);
        }
        catch(JAI::NoNewFrameException ex)
        {
          --i;
        }
      }
    }
    real_cam->stop();
    real_cam->close();
    tend = std::chrono::high_resolution_clock::now();
    ms = duration_cast<milliseconds>(tend-t0);
    std::cout << ms.count() << "\n";
    break;//read from camera;

  case Mode::DIR:
    fake_cam = JAI::FakeCamera::getCameraList().front();
    fake_cam->getImageSize(x, y);
    //BayerFilterStream bfs(x, y, 0, 0.8f, 0.7f, 0.9f);
    t0 = std::chrono::high_resolution_clock::now();
    if (fake_cam->open())
    {
      try
      {
        cv::Mat resize;
        while(1)
        {
          bfs.processImage(fake_cam->getNextFrame(), image);
          cv::resize(image, resize, cv::Size(800, 600));
          imshow("s", resize);
          cv::waitKey(1);
        }
      }
      catch(JAI::NoNewFrameException & ex)
      {
        //nothing to do, leaving
      } 
    }
    fake_cam->stop();
    fake_cam->close();
    tend = std::chrono::high_resolution_clock::now();
    ms = duration_cast<milliseconds>(tend-t0);
    std::cout << ms.count() << "\n";
    break; //read from dir;

  case Mode::FILE:
    try
    {
      //uchar mode = (uchar) atoi (argc[1]);
      //BayerFilterStream bfs(2456, 2058, mode, 0.8f, 0.7f, 0.9f);

      if (options.filename_out.empty())
      {
        std::cout << "Not implemented yet\n";
        //bfs.setFiles(options.filename, ); 
      }
      else
      {
        bfs.setFiles(options.filename, options.filename_out); 
      }
    }
    catch (OpenCLException e)
    {
      std::cout << e.getFullMessage () << "\n";
    }

    break; //read one file;
  
  case Mode::HELP:
    printHelp(argv[0]);
    break; //print help
  }

  return 0;
#if 0
  /*************** Fake camera *****************************/
  JAI::FakeCamera * cam = JAI::FakeCamera::getCameraList().front();
  int x, y;
  cv::Mat example_image = cam->getImageSize(x, y);
  BayerFilterStream bfs(x, y, 0, 0.8f, 0.7f, 0.9f);
  using std::chrono::milliseconds;
  using std::chrono::duration_cast;
  auto t0 = std::chrono::high_resolution_clock::now();
  if (cam->open())
  {
    try
    {
      cv::Mat resize;
      cv::Mat tmp(y, x, CV_8UC4);
      while(1)
      {
        bfs.processImage(cam->getNextFrame(), tmp);
        cv::resize(tmp, resize, cv::Size(800, 600));
        imshow("s", resize);
        cv::waitKey(1);
      }
    }
    catch(JAI::NoNewFrameException & ex)
    {
      //nothing to do
    } 
  }
  cam->stop();
  cam->close();
  auto t2 = std::chrono::high_resolution_clock::now();
  milliseconds ns2 = duration_cast<milliseconds>(t2-t0);
  std::cout << ns2.count() << "\n";
  return 0;

  /*************** Real camera *****************************/
  JAI::Camera * cam = JAI::Camera::getCameraList().front();
  int x, y;
  cam->getImageSize(x, y);
  BayerFilterStream bfs(x, y, 0, 0.8f, 0.7f, 0.9f);
  using std::chrono::milliseconds;
  using std::chrono::duration_cast;
  auto t0 = std::chrono::high_resolution_clock::now();
  if (cam->open() && cam->start())
  {
    cv::Mat resize;
    cv::Mat image = cv::Mat(y,x,CV_8UC4);
    for (int i=0; i < 100; ++i)
    {
      try
      {
        bfs.processImage(cam->getNextFrame(), image);
        cv::resize(image, resize, cv::Size(800, 600));
        imshow("s", resize);
        cv::waitKey(1);
      }
      catch(JAI::NoNewFrameException ex)
      {
        --i;
      }
    }
  }
  cam->stop();
  cam->close();
  auto t2 = std::chrono::high_resolution_clock::now();
  milliseconds ns2 = duration_cast<milliseconds>(t2-t0);
  std::cout << ns2.count() << "\n";
  return 0;
  //list_supported_image_formats ();
  return 0;
  
  try
  {
    int i = 2;
    int inf, outf;
    uchar mode = (uchar) atoi (argc[1]);
    BayerFilterStream bfs(2456, 2058, mode, 0.8f, 0.7f, 0.9f);
    while (i+2 <= argv)
    {
      inf = i++;
      outf = i++;

      bfs.setFiles(argc[inf], argc[outf]); 
    }
  }
  catch (OpenCLException e)
  {
    std::cout << e.getFullMessage () << "\n";
  }

  return 0;

  if (argv < 3)
  {
    std::cout << "Usage: " << argc[0] << " 0-3 filename.bmp [output_filename.bmp]\n";
    std::cout << "\n\t0-3 - bayer matix start mode\n";
    std::cout << "\twith specified output file dont show images, only save new file\n";
    return 1;
  }

  //std::cout << "Devices list:\n";
  //list_devices();

  try
  {
    uchar mode = (uchar) atoi (argc[1]);
    OpenCLImageFilter filter (argc[2], mode);
    if (argv > 3)
    {
      filter.saveOutputImage (argc[3]);
          std::cout << "Image '" << argc[3] << "' saved!\n";
          if (argv > 5)
            {
              filter.setInputImage (argc[4]);
              filter.saveOutputImage (argc[5]);
              std::cout << "Image '" << argc[5] << "' saved!\n";
            }
        }
      else
        {
          //cv::imshow ("Input Image", filter.getInputImage ());
          cv::imshow ("Output Image", filter.getOutputImage ());
          //cv::imshow ("Output Image original", filter.getOutputImageOpenCV ());
          //cv::imwrite("tmp_OpenCV.bmp", filter.getOutputImageOpenCV());

          while (cv::waitKey (10) < 0)
            {
              ;
            }
        }

    }
  catch (OpenCLException e)
    {
      std::cout << e.getFullMessage () << "\n";
    }


  return 0;
#endif
}

