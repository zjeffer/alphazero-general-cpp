#pragma once

#include "../NeuralNetwork/NeuralNetwork.hpp"

class Agent
{
private:
  NeuralNetwork m_neuralNetwork;

public:
  Agent();
  ~Agent() = default;
};