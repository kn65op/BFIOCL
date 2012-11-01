#include "gtest/gtest.h"

#include <OpenCLAlgorithm.h>

class SetDevice : public ::testing::Test
{
protected:
  void SetUp()
  {
    getDevice();
  }

  void getDevice()
  {
    device = OpenCLDevice::getDevices().front();
  }

  class OpenCLAlgorithmTest : public OpenCLAlgorithm
  {
  public:
    OpenCLAlgorithmTest() : OpenCLAlgorithm()
    {
    }

    OpenCLAlgorithmTest(const OpenCLDevice & dev) : OpenCLAlgorithm(dev)
    {

    }
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

class ConstructorTest : public SetDevice
{
protected:
  void SetUp()
  {
    al_ptr = NULL;
  }

  void TearDown()
  {
    if (al_ptr) delete al_ptr;
  }
  OpenCLAlgorithm *al_ptr;
};

TEST_F(SetDevice, SetProperDevice)
{
  ASSERT_NO_THROW(al.setDevice(device));
}

TEST_F(SetDevice, SetInvalidDevice)
{
  ASSERT_THROW(al.setDevice(OpenCLDevice()), OpenCLAlgorithmException);
}

TEST_F(ConstructorTest, NoParams)
{
  ASSERT_NO_THROW(al_ptr = new OpenCLAlgorithmTest());
}

TEST_F(ConstructorTest, ProperDevice)
{
  ASSERT_NO_THROW(al_ptr = new OpenCLAlgorithmTest(device));
}

TEST_F(ConstructorTest, InvalidDevice)
{
  ASSERT_THROW(al_ptr = new OpenCLAlgorithmTest(OpenCLDevice()), OpenCLAlgorithmException);
}

int main(int argc, char** argv) 
{ 
    testing::InitGoogleTest(&argc, argv); 
    RUN_ALL_TESTS(); 
}
