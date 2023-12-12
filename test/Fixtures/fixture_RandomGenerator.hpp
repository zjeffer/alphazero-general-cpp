#pragma once

#include <gtest/gtest.h>

#include "../../src/lib/Utilities/RandomGenerator.hpp"

struct RandomGeneratorFixture : public ::testing::Test
{
  RandomGeneratorFixture()
  {
    RandomGenerator::ResetSeed();
  }

  ~RandomGeneratorFixture() override = default;
};