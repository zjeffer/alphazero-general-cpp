#pragma once

#include <memory>

#include "../Environment/Environment.hpp"
#include "../NeuralNetwork/NeuralNetworkInterface.hpp"
#include "Node.hpp"

struct DirichletNoiseOptions
{
  bool  enable = true;
  float alpha;             // shape of the dirichlet distribution
  float beta;              // scale of the dirichlet distribution
  float dirichletFraction; // fraction of the dirichlet noise to add to the prior probabilities
};

class MCTS
{
private:
  std::shared_ptr<Node> m_root;
  DirichletNoiseOptions m_dirichletNoiseOptions;

public:
  MCTS(std::shared_ptr<Node> root, DirichletNoiseOptions const & dirichletNoiseOptions);
  ~MCTS() = default;

  void RunSimulations(uint numSimulations, NeuralNetworkInterface & network);

  std::shared_ptr<Node> GetRoot() const;
  std::shared_ptr<Move> GetBestMove(bool stochasticSearch) const;

private:
  static std::shared_ptr<Node> Select(std::shared_ptr<Node> const & root);
  static float                 Expand(std::shared_ptr<Node> const & node, NeuralNetworkInterface & network); // also does step 3: evaluation
  static void                  Backpropagate(std::shared_ptr<Node> const & node, float reward);

  static uint GetTreeDepth(Node * root);

  std::shared_ptr<Move> GetBestMoveStochastic() const;
  std::shared_ptr<Move> GetBestMoveDeterministic() const;

  void AddDirichletNoiseToRoot() const;
};