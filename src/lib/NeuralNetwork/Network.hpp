#pragma once

#include <tuple>

#include "Architecture.hpp"
#include "ConvBlock.hpp"
#include "NeuralNetwork.hpp"
#include "PolicyHead.hpp"
#include "ResidualBlock.hpp"
#include "ValueHead.hpp"

/**
 * @brief The full neural network architecture based on AlphaZero
 */
struct NetworkImpl : public torch::nn::Module
{
  NetworkImpl(NetworkArchitecture const & na)
  {
    convInput = register_module("convInput", ConvBlock(na.inputPlanes, na.filters));

    for (uint i = 0; i < na.residualBlocks; i++)
    {
      resBlocks.emplace_back(register_module("resBlock" + std::to_string(i), ResidualBlock(na.filters, na.filters, na.filters)));
    }

    valueHead  = register_module("valueHead", ValueHead(na.filters, na.valueFilters, na.width, na.height, na.filters));
    policyHead = register_module("policyHead", PolicyHead(na.filters, na.policyFilters, na.width, na.height, na.outputs));
  }

  std::pair<torch::Tensor, torch::Tensor> forward(torch::Tensor const & input)
  {
    // the first convolutional layer
    auto x = convInput(input);
    // all residual blocks
    for (auto & resBlock: resBlocks)
    {
      x = resBlock(x);
    }
    // return the two outputs
    return std::make_pair(policyHead(x), valueHead(x));
  }

  ConvBlock                  convInput = nullptr;
  std::vector<ResidualBlock> resBlocks;

  PolicyHead policyHead = nullptr;
  ValueHead  valueHead  = nullptr;
};

TORCH_MODULE(Network);