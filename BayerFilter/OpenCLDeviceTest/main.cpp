#include "gtest/gtest.h"

#include <OpenCLDevice.h>

/*class ECGControllerTest : public ::testing::Test
{
protected:

  ECGController controller;
};*/

TEST(OpenCLDevice, FullTrace)
{
  EXPECT_EQ(false, true);
}


int main(int argc, char** argv) 
{ 
    testing::InitGoogleTest(&argc, argv); 
    RUN_ALL_TESTS(); 
}
