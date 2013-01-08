#pragma once

#include <string>

enum class Mode
{
  CAMERA,
  DIR,
  FILE,
  HELP
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
  Mode & parseOptions(int argc, char * argv[]); //TODO: No return

public:
  Mode mode;
  std::string dirname;
  std::string filename;
  std::string filename_out;
};

