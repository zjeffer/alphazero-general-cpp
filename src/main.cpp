#include <iostream>

#include "Game.hpp"
#include "lib/Environment/Environment_TicTacToe.hpp"
#include "lib/Logging/Logger.hpp"
#include "lib/NeuralNetwork/NeuralNetwork.hpp"
#include "lib/Utilities/RandomGenerator.hpp"

Logger logger;

// create static generator
std::mt19937 RandomGenerator::generator(std::random_device{}());

int main()
{
  // reset random seed
  RandomGenerator::ResetSeed();

  // TODO: read the architecture from a json file
  auto ticTacToeArchitecture = NetworkArchitecture{
    .inputPlanes            = 3,  // 3 planes: 1 for X, 1 for 0, 1 for current player
    .width                  = 3,  // 3x3 board
    .height                 = 3,  // 3x3 board
    .residualBlocks         = 2,  // alphaZero uses 19, we use 2 for tic-tac-toe
    .filters                = 32, // 32 convolution filters per layer
    .policyOutputs          = 9,  // 1 * 3 * 3 = 9
    .policyFilters          = 2,  // alphaZero uses 2 policy filters
    .valueFilters           = 1,  // alphaZero uses 1 value filter
    .valueHeadLinearNeurons = 16, // alphaZero uses 256, we use 16 for tic-tac-toe
    // convolution options
    .kernelSize = 3, // 3x3 kernel
    .padding    = 1, // padding of 1
    .stride     = 1, // stride of 1
  };
  NeuralNetwork neuralNetwork1 = NeuralNetwork(ticTacToeArchitecture);
  NeuralNetwork neuralNetwork2 = NeuralNetwork(ticTacToeArchitecture);

  // TODO: read the agent names from a json file
  auto agent1 = std::make_unique<Agent>("X", neuralNetwork1);
  auto agent2 = std::make_unique<Agent>("0", neuralNetwork2);

  // TODO: read the options from a json file
  auto gameOptions = GameOptions{
    .saveMemory       = true,
    .maxMoves         = 9,
    .simsPerMove      = 200,
    .stochasticSearch = true,
  };
  Game game = Game(std::make_unique<EnvironmentTicTacToe>(), std::move(agent1), std::move(agent2), gameOptions);

  // self-play
  game.PlayGame();

  return 0;
}
