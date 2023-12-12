#pragma once

#include <memory>

#include "../Environment/Environment.hpp"
#include "../NeuralNetwork/NeuralNetworkInterface.hpp"
#include "Node.hpp"

class MCTS
{
private:
  std::shared_ptr<Node> m_root;

public:
  MCTS(std::shared_ptr<Node> root);
  ~MCTS() = default;

  void RunSimulations(uint numSimulations, NeuralNetworkInterface & network);

  std::shared_ptr<Node> GetRoot() const;
  std::shared_ptr<Move> GetBestMove(bool stochasticSearch) const;

private:
  static std::shared_ptr<Node> Select(std::shared_ptr<Node> const & root);
  static float                 Expand(std::shared_ptr<Node> const & node, NeuralNetworkInterface & network); // also does step 3: evaluation
  static void                  Backpropagate(std::shared_ptr<Node> const & node, float reward);

  static uint GetTreeDepth(Node * root);
};