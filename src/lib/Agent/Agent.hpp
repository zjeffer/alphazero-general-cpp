#pragma once

#include "../MCTS/MCTS.hpp"
#include "../NeuralNetwork/NeuralNetwork.hpp"

class Agent
{
private:
  std::string   m_name;
  NeuralNetwork m_neuralNetwork;

public:
  Agent(std::string name, NeuralNetwork & neuralNetwork);
  ~Agent() = default;

  void RunSimulations(std::shared_ptr<MCTS> const & mcts, uint numSimulations);
};