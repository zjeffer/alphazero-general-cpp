#pragma once

#include <filesystem>
#include <string>

#include "Architecture/Network.hpp"

class NeuralNetworkInterface
{
public:
  virtual ~NeuralNetworkInterface() = default;

  virtual Network GetNetwork() = 0;

  virtual std::pair<torch::Tensor, torch::Tensor> Predict(torch::Tensor & input) = 0;

  virtual bool                  LoadModel(std::filesystem::path const & folder) = 0;
  virtual std::filesystem::path SaveModel(std::filesystem::path const & folder) = 0;
};