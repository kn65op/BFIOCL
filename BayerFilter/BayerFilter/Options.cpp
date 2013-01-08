#include "Options.h"

Options::Options(void)
{
  opencv = false;
}


Options::~Options(void)
{
}

void Options::parseOptions(int argc, char * argv[])
{
  if (argc == 1)
  {
    mode = Mode::CAMERA;
  }
  else
  {
    std::string param = argv[1];
    if (param == "-c")
    {
      mode = Mode::CAMERA;
    }
    else if(param == "-d")
    {
      mode = Mode::DIR;
      if (argc < 4)
      {
        mode = Mode::HELP;
      }
      else
      {
        dirname = argv[2];
        filename = argv[3];
      }
      if (argc == 5 && (param = argv[4]) == "--openCV")
      {
        opencv = true;
      }
    }
    else if(param == "-f")
    {
      mode = Mode::FILE;
      if (argc < 3)
      {
        mode = Mode::HELP;
      }
      else
      {
        filename = argv[2];
      }
      if (argc > 4 && (param = argv[3]) == "-o")
      {
        filename_out = argv[4];
      }
      if (argc > 5 && (param = argv[5]) == "--openCV")
      {
        opencv = true;
      }
    }
    else
    {
      mode = Mode::HELP;
    }
  }
}
