#include <iostream>

#include "Game.hpp"
#include "lib/Environment/Environment_TicTacToe.hpp"
#include "lib/Logging/Logger.hpp"

Logger logger;

int main()
{
  // TODO: read the architecture from a json file
  auto ticTacToeArchitecture = NetworkArchitecture{
    .inputPlanes    = 3,  // 3 planes: 1 for X, 1 for 0, 1 for current player
    .width          = 3,  // 3x3 board
    .height         = 3,  // 3x3 board
    .outputs        = 9,  // 1 * 3 * 3 = 9
    .filters        = 32, // 32 convolution filters per layer
    .policyFilters  = 2,  // alphaZero uses 2 policy filters
    .valueFilters   = 1,  // alphaZero uses 1 value filter
    .residualBlocks = 4,  // alphaZero uses 19, we use 4 for tic-tac-toe
  };
  NeuralNetwork neuralNetwork1 = NeuralNetwork(ticTacToeArchitecture);
  NeuralNetwork neuralNetwork2 = NeuralNetwork(ticTacToeArchitecture);

  // TODO: read the agent names from a json file
  auto agent1 = std::make_unique<Agent>("X", neuralNetwork1);
  auto agent2 = std::make_unique<Agent>("0", neuralNetwork2);

  // TODO: read the options from a json file
  auto gameOptions = GameOptions{
    .saveMemory  = true,
    .maxMoves    = 9,
    .simsPerMove = 100,
  };
  Game game = Game(std::make_unique<EnvironmentTicTacToe>(), std::move(agent1), std::move(agent2), gameOptions);

  // self-play
  game.PlayGame();

  return 0;
}
