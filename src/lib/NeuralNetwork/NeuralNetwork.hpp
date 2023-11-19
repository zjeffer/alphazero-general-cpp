#pragma once

#include <filesystem>
#include <string>

#include "../Environment/Environment.hpp"
#include "Network.hpp"

/**
 * @brief The NeuralNetwork class holds the torch Network to run inference with.
 * It also features some methods to load & save the model, and
 * convert a board to an input state for use with the network.
 */
class NeuralNetwork
{
public:
  NeuralNetwork(NetworkArchitecture const & architecture);
  ~NeuralNetwork() = default;

  Network GetNetwork();

  std::pair<torch::Tensor, torch::Tensor> Predict(torch::Tensor & input);

  bool                  LoadModel(std::filesystem::path const & path);
  std::filesystem::path SaveModel(std::filesystem::path const & path);

private:
  torch::Device m_device;
  Network       m_net = nullptr;
};