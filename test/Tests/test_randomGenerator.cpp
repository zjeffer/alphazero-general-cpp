#include <gtest/gtest.h>

#include "../../src/lib/Environment/Environment_TicTacToe.hpp"
#include "../../src/lib/MCTS/Node.hpp"
#include "../../src/lib/NeuralNetwork/NeuralNetwork.hpp"
#include "../../src/lib/Utilities/Functions.hpp"
#include "../../src/lib/Utilities/RandomGenerator.hpp"
#include "../Fixtures/fixture_RandomGenerator.hpp"

std::mt19937 RandomGenerator::generator(std::random_device{}());

TEST_F(RandomGeneratorFixture, GenerateGamma_CheckRange)
{
  for (int i = 0; i < 1000; i++)
  {
    float gamma = RandomGenerator::GenerateGamma(0.3F, 1.0F);
    ASSERT_GE(gamma, 0.0F) << "Gamma is less than 0.0";
  }
}

// TODO: rewrite with RandomGenerator's own CalculateDirichletNoise function, instead of AddDirichletNoise
/* TEST_F(RandomGeneratorFixture, DirichletNoise)
{
  // TODO: extract the creation of the network to a fixture
  // TODO: extract the creation of the environment and initial MCTS tree (with children) to a fixture

  // set up new tree with 1 root node and all its children
  auto env  = std::make_shared<EnvironmentTicTacToe>();
  Node root = Node(env);

  auto ticTacToeArchitecture = NetworkArchitecture{
    .inputPlanes            = 3,  // 3 planes: 1 for X, 1 for 0, 1 for current player
    .width                  = 3,  // 3x3 board
    .height                 = 3,  // 3x3 board
    .residualBlocks         = 2,  // alphaZero uses 19 for chess, we use 2 for tic-tac-toe
    .filters                = 32, // 32 convolution filters per layer
    .policyOutputs          = 9,  // 1 * 3 * 3 = 9
    .policyFilters          = 2,  // alphaZero uses 2 policy filters
    .valueFilters           = 1,  // alphaZero uses 1 value filter
    .valueHeadLinearNeurons = 16, // alphaZero uses 256 for chess, we use 16 for tic-tac-toe
    // convolution options
    .kernelSize = 3, // 3x3 kernel
    .padding    = 1, // padding of 1
    .stride     = 1, // stride of 1
  };
  NeuralNetwork network = NeuralNetwork(ticTacToeArchitecture);

  auto input                       = root.GetEnvironment()->BoardToInput();
  auto [policyOutput, valueOutput] = network.Predict(input);
  policyOutput                     = policyOutput.view({3, 3});

  auto moves = root.GetEnvironment()->GetValidMoves();
  ASSERT_FALSE(moves.empty()) << "There are no valid moves";

  // for this test, make the prior probability of the first move 0.0
  moves[0]->SetPriorProbability(0.0F);

  for (auto const & move: moves)
  {
    move->SetPriorProbability(policyOutput[move->GetRow()][move->GetColumn()].item<float>());
    auto newEnv = std::make_shared<EnvironmentTicTacToe>(*env);
    newEnv->MakeMove(*move);
    root.AddChild(std::make_shared<Node>(newEnv, std::make_shared<Node>(root), move));
  }

  float sumBeforeDirichletNoise = 0.0F;
  for (auto const & child: root.GetChildren())
  {
    sumBeforeDirichletNoise += child->GetPriorProbability();
  }
  ASSERT_NE(sumBeforeDirichletNoise, 0.0F);

  utils::AddDirichletNoise(&root);

  // check that the sum of the noise vector is the same
  float sumAfterDirichletNoise = 0.0F;
  for (auto const & child: root.GetChildren())
  {
    sumAfterDirichletNoise += child->GetPriorProbability();
  }

  ASSERT_FLOAT_EQ(sumBeforeDirichletNoise, sumAfterDirichletNoise);

  for (auto const & child: root.GetChildren())
  {
    ASSERT_GT(child->GetPriorProbability(), 0.0F);
  }
} */

TEST_F(RandomGeneratorFixture, StochasticSample)
{
  const uint         sampleTimes = 10000;
  std::vector<float> probabilities{0.90F, 0.01F, 0.01F, 0.01F, 0.01F, 0.01F, 0.01F, 0.01F, 0.01F, 0.01F, 0.01F};

  // sample
  std::vector<int> counts(11, 0);
  for (int i = 0; i < sampleTimes; i++)
  {
    int index = RandomGenerator::StochasticSample(probabilities);
    counts[index]++;
  }

  // based on the probabilities, the counts should be roughly the same as the probabilities * sampleTimes, with a small tolerance
  for (int i = 0; i < counts.size(); i++)
  {
    ASSERT_NEAR(counts[i], probabilities[i] * sampleTimes, 0.05F * sampleTimes);
  }
}
