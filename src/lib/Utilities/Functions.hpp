#pragma once

#include <random>

#include "../MCTS/Node.hpp"
#include "RandomGenerator.hpp"

namespace utils
{

namespace
{

// tunable parameters
auto constexpr ALPHA              = 0.3F;
auto constexpr DIRICHLET_FRACTION = 0.25F;

} // namespace

void AddDirichletNoise(Node * root)
{
  auto const &       children = root->GetChildren();
  std::vector<float> noise(children.size());
  for (auto & n: noise)
  {
    n = RandomGenerator::GenerateGamma(ALPHA, 1.0F);
  }

  float sum = std::accumulate(noise.begin(), noise.end(), 0.0F);
  for (auto & n: noise)
  {
    n /= sum;
  }

  for (uint i = 0; i < children.size(); i++)
  {
    children[i]->SetPriorProbability(children[i]->GetPriorProbability() * (1.0F - DIRICHLET_FRACTION) + noise[i] * DIRICHLET_FRACTION);

    // TODO: remove
    // ensure the root's children have the correct prior probabilities
    assert(children[0]->GetPriorProbability() == root->GetChildren()[0]->GetPriorProbability());
  }

  // by checking the first child's prior probability
}

} // namespace utils