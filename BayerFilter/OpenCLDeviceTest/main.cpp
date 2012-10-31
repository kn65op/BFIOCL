#include "gtest/gtest.h"

#include <OpenCLDevice.h>

class DeviceParameters : public ::testing::Test
{
protected:
  void getDevs()
  {
    devs = OpenCLDevice::getDevices();
  }
  void SetUp()
  {
    getDevs();
  }
  std::list<OpenCLDevice> devs;
};

class DeviceContext : public DeviceParameters
{
};

class CommandQueue : public DeviceParameters
{
};

class BuildProgram : public DeviceParameters
{
protected:
  void SetUp()
  {
    getDevs();
    source = "\n" \
"__kernel void square(                                                       \n" \
"   __global float* input,                                              \n" \
"   __global float* output,                                             \n" \
"   const unsigned int count)                                           \n" \
"{                                                                      \n" \
"   int i = get_global_id(0);                                           \n" \
"   if(i < count)                                                       \n" \
"       output[i] = input[i] * input[i];                                \n" \
"}                                                                      \n" \
"\n";
    filename = "test.cl";
  }
  std::string filename;
  std::string source;
};

TEST(QueryingPlatforms, NumberOfDevices)
{
  EXPECT_GT(OpenCLDevice::getDevices().size(), (unsigned int)0);
}

TEST_F(DeviceParameters, QueryNameNoThrow)
{
  ASSERT_GT(devs.size(), (unsigned int)0);
  EXPECT_NO_THROW(devs.front().getName());
}

TEST_F(DeviceParameters, QueryPlatformNameNoThrow)
{
  ASSERT_GT(devs.size(), (unsigned int)0);
  EXPECT_NO_THROW(devs.front().getName());
}

TEST_F(DeviceContext, GetContextNoThrow)
{
  ASSERT_GT(devs.size(), (unsigned int)0);
  EXPECT_NO_THROW(devs.front().getContext());
}

TEST_F(CommandQueue, GetCommandQueueNoThrow)
{
  ASSERT_GT(devs.size(), (unsigned int)0);
  EXPECT_NO_THROW(devs.front().getCommandQueue());
}

TEST_F(BuildProgram, BuildFromSourceNoThrow)
{
  ASSERT_GT(devs.size(), (unsigned int)0);
  EXPECT_NO_THROW(devs.front().createAndBuildProgramFromFile(filename));
}

TEST_F(BuildProgram, BuildFromFileNoThrow)
{
  ASSERT_GT(devs.size(), (unsigned int)0);
  EXPECT_NO_THROW(devs.front().createAndBuildProgramFromSource(source));
}

int main(int argc, char** argv) 
{ 
    testing::InitGoogleTest(&argc, argv); 
    RUN_ALL_TESTS(); 
}
