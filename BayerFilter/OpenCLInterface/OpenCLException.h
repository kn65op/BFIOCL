#pragma once

#include <string>

/**
 * @brief Exception class for OpenCLInterface
 */
class OpenCLException
{
public:
  /**
   * @param m Message.
   * @param err Error code. Put 0 if it is not an OpenCL internal error.
   */
  OpenCLException(std::string m, int err) : error_no(err), message(m) {}
  std::string getFullMessage();
private:
  OpenCLException(void);
  int error_no;
  std::string message;
  std::string getInfoFromErrorCode();
};
