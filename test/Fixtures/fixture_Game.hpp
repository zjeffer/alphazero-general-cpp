#pragma once
#include <gtest/gtest.h>

#include "../../src/Game.hpp"
#include "../../src/lib/Environment/Environment_TicTacToe.hpp"
#include "../Mocks/mock_NeuralNetwork.hpp"

GameOptions const GAME_OPTIONS = {
  .saveMemory       = false,
  .maxMoves         = 9,
  .simsPerMove      = 100,
  .stochasticSearch = false, // don't test stochastic search
  .memoryFolder     = "test/data",
};

struct GameFixture : public ::testing::Test
{
  GameFixture()
    : environment(std::make_shared<EnvironmentTicTacToe>())
    , neuralNetwork(NeuralNetworkMock())
    , agent1(std::make_unique<Agent>("X", neuralNetwork))
    , agent2(std::make_unique<Agent>("O", neuralNetwork))
    , game(environment, std::move(agent1), std::move(agent2), GAME_OPTIONS)
  {
  }

  ~GameFixture() override = default;

  std::shared_ptr<Environment> environment;
  NeuralNetworkMock            neuralNetwork;
  std::unique_ptr<Agent>       agent1;
  std::unique_ptr<Agent>       agent2;
  Game                         game;
};