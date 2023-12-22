#pragma once

#include <gtest/gtest.h>

#include "../../src/lib/MCTS/MCTS.hpp"
#include "../Mocks/mock_Environment.hpp"

using ::testing::_;

struct MCTSFixture : public ::testing::Test
{
  MCTSFixture()
    : env(std::make_shared<MockEnvironment>())
    , mcts(std::make_shared<Node>(env), DirichletNoiseOptions{0.3F, 1.0F})
  {
  }

  ~MCTSFixture() override = default;

  std::shared_ptr<Environment> env;
  MCTS                         mcts;
};
