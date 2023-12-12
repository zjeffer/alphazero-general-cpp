
#include <gtest/gtest.h>

#include "../src/lib/Logging/Logger.hpp"

Logger logger;

int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
