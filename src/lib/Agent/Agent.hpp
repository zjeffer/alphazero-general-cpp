#pragma once

#include "../MCTS/MCTS.hpp"
#include "../NeuralNetwork/NeuralNetworkInterface.hpp"

class Agent
{
private:
  std::string              m_name;
  NeuralNetworkInterface & m_neuralNetwork;

public:
  Agent(std::string name, NeuralNetworkInterface & neuralNetwork);
  ~Agent() = default;

  void RunSimulations(std::shared_ptr<MCTS> const & mcts, uint numSimulations);
};