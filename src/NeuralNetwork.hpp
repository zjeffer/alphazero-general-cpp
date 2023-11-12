#pragma once

#include <filesystem>
#include <string>

#include "lib/Environment/Environment.hpp"
#include "lib/NeuralNetwork/Network.hpp"

/**
 * @brief The NeuralNetwork class holds the torch Network to run inference with.
 * It also features some methods to load & save the model, and
 * convert a board to an input state for use with the network.
 */
class NeuralNetwork
{
public:
  NeuralNetwork();
  ~NeuralNetwork() = default;

  Network GetNetwork();

  static torch::Tensor BoardToInput(torch::Tensor const & board, Player player, int inputPlanes);
  torch::Tensor        BoardToInput(std::shared_ptr<Environment> const & env);

  std::pair<torch::Tensor, torch::Tensor> Predict(torch::Tensor const & input);

  bool                  LoadModel(std::filesystem::path const & path);
  std::filesystem::path SaveModel(std::filesystem::path const & path);

private:
  torch::Device m_device;
  Network       m_net = nullptr;
};