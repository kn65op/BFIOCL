#pragma once

#include <string>

class OpenCLException
{
public:
  OpenCLException(std::string m, int err) : message(m), error_no(err) {}
  std::string getInfoFromErrorCode();
  std::string getFullMessage();
private:
  OpenCLException(void);
  int error_no;
  std::string message;
};

