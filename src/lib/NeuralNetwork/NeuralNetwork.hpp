#pragma once

#include <filesystem>
#include <string>

#include "../Environment/Environment.hpp"
#include "Device.hpp"
#include "NeuralNetworkInterface.hpp"

/**
 * @brief The NeuralNetwork class holds the torch Network to run inference with.
 * It also features some methods to load & save the model, and
 * convert a board to an input state for use with the network.
 */
class NeuralNetwork : public NeuralNetworkInterface
{
private:
  Device & m_device;
  Network  m_net = nullptr;

public:
  NeuralNetwork(NetworkArchitecture const & architecture);
  NeuralNetwork(NetworkArchitecture const & architecture, std::filesystem::path const & path);
  ~NeuralNetwork() override = default;

  Network GetNetwork() override;

  std::pair<torch::Tensor, torch::Tensor> Predict(torch::Tensor & input) override;

  bool                  LoadModel(std::filesystem::path const & path) override;
  std::filesystem::path SaveModel(std::filesystem::path const & path) override;

private:
  NeuralNetwork(); // private default constructor so we can easily initialize common stuff in the other constructors
};