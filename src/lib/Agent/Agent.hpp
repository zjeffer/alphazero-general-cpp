#pragma once

#include "../NeuralNetwork/NeuralNetwork.hpp"

class Agent
{
private:
  std::string   m_name;
  NeuralNetwork m_neuralNetwork;

public:
  Agent(std::string name, NeuralNetwork const & neuralNetwork);
  ~Agent() = default;
};