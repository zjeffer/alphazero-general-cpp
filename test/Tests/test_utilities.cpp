#include <gtest/gtest.h>

#include "../../src/lib/Environment/Environment_TicTacToe.hpp"
#include "../../src/lib/MCTS/Node.hpp"
#include "../../src/lib/NeuralNetwork/NeuralNetwork.hpp"
#include "../../src/lib/Utilities/Functions.hpp"
#include "../../src/lib/Utilities/RandomGenerator.hpp"

std::mt19937 RandomGenerator::generator(std::random_device{}());

struct RandomGeneratorFixture : public ::testing::Test
{
  RandomGeneratorFixture()
  {
    RandomGenerator::ResetSeed();
  }

  ~RandomGeneratorFixture() override = default;
};

TEST_F(RandomGeneratorFixture, GenerateGamma_CheckRange)
{
  for (int i = 0; i < 1000; i++)
  {
    float gamma = RandomGenerator::GenerateGamma(0.3F, 1.0F);
    ASSERT_GE(gamma, 0.0F) << "Gamma is less than 0.0";
  }
}

TEST_F(RandomGeneratorFixture, DirichletNoise)
{
  // TODO: extract the creation of the network to a fixture
  // TODO: extract the creation of the environment and initial MCTS tree (with children) to a fixture

  // set up new tree with 1 root node and all its children
  auto   env  = std::make_shared<EnvironmentTicTacToe>();
  Node * root = new Node(env);

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

  auto input                       = root->GetEnvironment()->BoardToInput();
  auto [policyOutput, valueOutput] = network.Predict(input);
  policyOutput                     = policyOutput.view({3, 3});

  auto moves = root->GetEnvironment()->GetValidMoves();
  ASSERT_FALSE(moves.empty()) << "There are no valid moves";
  for (auto const & move: moves)
  {
    move->SetPriorProbability(policyOutput[move->GetRow()][move->GetColumn()].item<float>());
    auto newEnv = std::make_shared<EnvironmentTicTacToe>(*env);
    newEnv->MakeMove(*move);
    root->AddChild(std::make_unique<Node>(newEnv, move));
  }

  float sumBeforeDirichletNoise = 0.0F;
  for (auto const & child: root->GetChildren())
  {
    sumBeforeDirichletNoise += child->GetPriorProbability();
  }

  float firstChildProbabilityBeforeDirichletNoise = root->GetChildren()[0]->GetPriorProbability();

  utils::AddDirichletNoise(root);

  // check that the sum of the noise vector is the same
  float sumAfterDirichletNoise = 0.0F;
  for (auto const & child: root->GetChildren())
  {
    sumAfterDirichletNoise += child->GetPriorProbability();
  }

  float firstChildProbabilityAfterDirichletNoise = root->GetChildren()[0]->GetPriorProbability();

  ASSERT_FLOAT_EQ(sumBeforeDirichletNoise, sumAfterDirichletNoise);
  ASSERT_NE(sumBeforeDirichletNoise, 0.0F);
  ASSERT_NE(firstChildProbabilityBeforeDirichletNoise, firstChildProbabilityAfterDirichletNoise);
}
