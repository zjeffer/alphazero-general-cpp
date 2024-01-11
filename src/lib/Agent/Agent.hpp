#pragma once

#include "../MCTS/MCTS.hpp"
#include "../NeuralNetwork/NeuralNetworkInterface.hpp"

struct AgentOptions
{
  std::vector<std::string> agentNames;

  AgentOptions(std::filesystem::path const & path)
  {
    auto config = Configuration(path);
    agentNames  = config.Get<std::vector<std::string>>("agent_names");
  }
};

class Agent
{
private:
  std::string                             m_name;
  std::shared_ptr<NeuralNetworkInterface> m_neuralNetwork;

public:
  Agent(std::string name, std::shared_ptr<NeuralNetworkInterface> neuralNetwork);
  ~Agent() = default;

  void RunSimulations(std::shared_ptr<MCTS> const & mcts, uint numSimulations);
};