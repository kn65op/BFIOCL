#pragma once

#include "OpenCLDevice.h"
#include "OpenCLException.h"

struct OpenCLAlgorithmException : public OpenCLException
{
  OpenCLAlgorithmException(std::string m, int e = 0) : OpenCLException(m,e) {}
};

class OpenCLAlgorithmParams {
public:
  virtual ~OpenCLAlgorithmParams() {};
};

/**
 * @brief Iterface for OpenCL algorithm.
 */
class OpenCLAlgorithm
{
public:
  OpenCLAlgorithm(void);
  OpenCLAlgorithm(const OpenCLDevice & dev);
  virtual ~OpenCLAlgorithm(void);

  virtual void setDevice(const OpenCLDevice & dev);
  virtual void enqueueNDRangeKernelWithTimeMeasurment(cl_uint work_dim, size_t * global_work_offset, const size_t *global_work_size, const size_t *local_work_size, cl_uint num_events_in_wait_list);
  double getTimeConsumed() const;
  virtual void prepare(size_t di_size, size_t do_size);
  virtual void releaseMem() = 0; //TODO: unpure 

  //pure virtual functions
  virtual void setParams(const OpenCLAlgorithmParams & params) = 0;
  virtual void run(const unsigned char * data_input, size_t di_size, unsigned char * data_output, size_t do_size) = 0;
  virtual void setKernelArgs(size_t di_size, size_t do_size) = 0;
  
protected:

  OpenCLDevice device;
 
  cl_kernel kernel;
  cl_program program;  
  cl_command_queue command_queue;
  cl_context context;

  //kernel name
  std::string kernel_name;
  //source file - to release version change it to kernel code 
  std::string source_file;

  //time consumed by kernel in ms
  double total_time;

  //cl_mem for input and output 
  cl_mem input, output;

private:
  void setDevice();

};

class OpenCLImageAlgorithm : virtual public OpenCLAlgorithm
{
protected:
  //size of elements inputs and outputs kernel
  size_t input_element_size;
  size_t output_element_size;
  //format of input and output
  cl_image_format input_format;
  cl_image_format output_format;
  
  virtual void prepareForStream(cl_command_queue cc, cl_context c);
  virtual void runStream(const size_t * global_work_size);
  virtual void copyDataToGPUStream() = 0;

  //pure virtual functions
  virtual void setKernelArgsForStream() = 0;

  friend class OpenCLAlgorithmsStream;
};