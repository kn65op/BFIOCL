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

  Mode & parseOptions(int argc, char * argv[]);

public:
  Mode mode;
  std::string dirname;
  std::string filename;
  std::string filename_out;
};

