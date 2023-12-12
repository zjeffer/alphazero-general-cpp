#include "Agent.hpp"

#include <utility>

Agent::Agent(std::string name, NeuralNetworkInterface & neuralNetwork)
  : m_name(std::move(name))
  , m_neuralNetwork(neuralNetwork)
{
}

void Agent::RunSimulations(std::shared_ptr<MCTS> const & mcts, uint numSimulations)
{
  mcts->RunSimulations(numSimulations, m_neuralNetwork);
}
