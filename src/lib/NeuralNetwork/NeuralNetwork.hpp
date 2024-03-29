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
  Device &            m_device;
  Network             m_net = nullptr;
  NetworkArchitecture m_architecture;

public:
  NeuralNetwork(NetworkArchitecture const & architecture);
  NeuralNetwork(std::filesystem::path const & folder);
  ~NeuralNetwork() override = default;

  Network GetNetwork() override;

  std::pair<torch::Tensor, torch::Tensor> Predict(torch::Tensor & input) override;

  void                  LoadModel(std::filesystem::path const & folder) override;
  std::filesystem::path SaveModel(std::filesystem::path const & folder) override;

private:
  NeuralNetwork(); // private default constructor so we can easily initialize common stuff in the other constructors
};