#include "gtest/gtest.h"

#include <OpenCLAlgorithm.h>

class SetDevice : public ::testing::Test
{
protected:
  void SetUp()
  {
    device = OpenCLDevice::getDevices().front();
  }

  class OpenCLAlgorithmTest : public OpenCLAlgorithm
  {
  public:
    void setParams(const OpenCLAlgorithmParams & params)
    {
      throw OpenCLAlgorithmException("Not implemented", 0);
    }
    void prepare()
    {
      throw OpenCLAlgorithmException("Not implemented", 0);
    }
    void run(const char * data_input, size_t di_size, char * data_output, size_t do_size)
    {
      throw OpenCLAlgorithmException("Not implemented", 0);
    }
  };

  OpenCLDevice device;
  OpenCLAlgorithmTest al;
};

TEST_F(SetDevice, SetProperDevice)
{
  ASSERT_NO_THROW(al.setDevice(device));
}


int main(int argc, char** argv) 
{ 
    testing::InitGoogleTest(&argc, argv); 
    RUN_ALL_TESTS(); 
}
