#pragma once

#include <OpenCLIntToFloat.h>
#include <Camera.h>

#include <string>

enum class Mode
{
  CAMERA,
  DIR,
  FILE,
  HELP,
  NONE
};

class Options
{
public:
  Options(void);
  ~Options(void);

  /**
   * Parse options sent to applications.
   * @param argc Number of argument.
   * @param argv Argument values.
   */
  void parseOptions(int argc, char * argv[]); //TODO: No return

public:
  Mode mode;
  std::string dirname;
  std::string filename;
  std::string filename_out;
  bool opencv;

  float r, g, b;

  OpenCLIntToFloatMode input_mode;
  JAI::OutputMode camera_mode;
};

