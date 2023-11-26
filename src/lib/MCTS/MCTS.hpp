#pragma once

#include <memory>

#include "../Environment/Environment.hpp"
#include "../NeuralNetwork/NeuralNetwork.hpp"
#include "Node.hpp"

class MCTS
{
private:
  std::shared_ptr<Node> m_root;

public:
  MCTS(std::shared_ptr<Node> root);
  ~MCTS() = default;

  void   RunSimulations(uint numSimulations, NeuralNetwork & network);
  Node * GetRoot() const;

  Move const & GetBestMove(bool stochasticSearch) const;

private:
  Node * Select(Node * root);
  float  Expand(Node * node, NeuralNetwork & network); // also does step 3: evaluation
  void   Backpropagate(Node * node, float reward);
};