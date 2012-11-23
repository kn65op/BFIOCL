#pragma once

#include "OpenCLAlgorithm.h"

#include <list>

struct OpenCLAlgorithmsStreamException : public OpenCLException
{
  OpenCLAlgorithmsStreamException(std::string m, int e = 0) : OpenCLException(m,e) {}
};

class OpenCLAlgorithmsStream
{
public:
  OpenCLAlgorithmsStream(void);
  ~OpenCLAlgorithmsStream(void);

  void pushAlgorithm(OpenCLImageAlgorithm * al);
  void clearAlgorithms();

  void setDataSize(size_t w, size_t h);
  void prepare();
  void processImage(void * data_input, void * data_output);
  
private:
  OpenCLDevice device;

  std::list<OpenCLImageAlgorithm*> algorithms;
  size_t width, height;
  cl_mem input, output;
  std::list<cl_mem> mems;
  cl_context context;
  cl_command_queue command_queue;
};


