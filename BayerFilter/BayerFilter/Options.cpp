#include "Options.h"

Options::Options(void)
{
  opencv = false;
  r = b = g = 1.0f;
}


Options::~Options(void)
{
}

void Options::parseOptions(int argc, char * argv[])
{
  mode = Mode::NONE;
  if (argc == 1)
  {
    mode = Mode::CAMERA;
  }
  int i = 1;
  while (i < argc)
  {
    std::string param = argv[i];
    if (param == "-c") //mode camera
    {
      if (mode == Mode::NONE)
      {
        mode = Mode::CAMERA;
      }
      else
      {
        mode = Mode::HELP;
        return;
      }
      ++i;
    }
    else if(param == "-d")
    {
      if (mode == Mode::NONE)
      {
        mode = Mode::DIR;
      }
      else
      {
        mode = Mode::HELP;
        return;
      }
      if (argc < i + 3)
      {
        mode = Mode::HELP;
        return;
      }
      else
      {
        dirname = argv[i + 1];
        filename = argv[i + 2];
      }
      i += 3;
    }
    else if(param == "-f")
    {
      if (mode == Mode::NONE)
      {
        mode = Mode::FILE;
      }
      else
      {
        mode = Mode::HELP;
        return;
      }
      if (argc < i + 2)
      {
        mode = Mode::HELP;
        return;
      }
      else
      {
        filename = argv[i + 1];
      }
      i += 2;
    }
    else if (param == "-o")
    {
      if (argc < i + 2)
      {
        mode = Mode::HELP;
        return;
      }
      else
      {
        filename_out = argv[i + 1];
      }
      i += 2;
    }
    else if (param == "--openCV")
    {
      opencv = true;
      ++i;
    }
    else if (param == "--wb")
    {
      if (argc < i + 4)
      {
        mode = Mode::HELP;
        return;
      }
      r = atof(argv[i + 1]);
      g = atof(argv[i + 2]);
      b = atof(argv[i + 3]);
      i += 4;
    }
    else
    {
      mode = Mode::HELP;
      return;
    }
  }
  if (mode == Mode::NONE)
  {
    mode = Mode::CAMERA;
  }
}
