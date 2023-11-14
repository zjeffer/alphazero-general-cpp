#include "Agent.hpp"

#include <utility>

Agent::Agent(std::string name, NeuralNetwork const & neuralNetwork)
  : m_name(std::move(name))
  , m_neuralNetwork(neuralNetwork)
{
}
