#include "Options.h"

#include <iostream>

Options::Options(void)
{
  input_mode = OpenCLIntToFloatMode::UINT_8;
  camera_mode = JAI::OutputMode::UNIT_8;
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
      if (argc < i + 3)
      {
        mode = Mode::HELP;
        return;
      }
      else
      {
        filename_out = argv[i + 2];
        filename = argv[i + 1];
      }
      i += 3;
    }
    else if (param == "-o")
    {
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
    else if (param == "-i")
    {
      if (argc < i + 2)
      {
        mode = Mode::HELP;
        return;
      }
      switch (atoi(argv[i + 1])) //mode can be 8, 10, 12, 16
      {
        case 8 :
          input_mode = OpenCLIntToFloatMode::UINT_8;
          camera_mode = JAI::OutputMode::UNIT_8;
          break;
        case 10 :
          input_mode = OpenCLIntToFloatMode::UINT_10;
          camera_mode = JAI::OutputMode::UNIT_10;
          break;
        case 12 :
          input_mode = OpenCLIntToFloatMode::UINT_12;
          camera_mode = JAI::OutputMode::UNIT_12;
          break;
        case 16 :
          input_mode = OpenCLIntToFloatMode::UINT_16;
          camera_mode = JAI::OutputMode::UNIT_16;
          break;
      default:
        mode = Mode::HELP;
        std::cout << "Invalid value for input mode: " << atoi(argv[i + 1]) << "\n";
      }
      i += 2;
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
