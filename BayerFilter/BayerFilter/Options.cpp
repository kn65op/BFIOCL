#include "Options.h"

Options::Options(void)
{
}


Options::~Options(void)
{
}

Mode & Options::parseOptions(int argc, char * argv[])
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
      if (argc > 4) //TODO: dorobiæ sparwdzanie lepsze
      {
        filename_out = argv[4];
      }
    }
    else
    {
      mode = Mode::HELP;
    }
  }
  return mode;
}
