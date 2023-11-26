#pragma once

#include <tuple>

#include "ConvBlock.hpp"
#include "PolicyHead.hpp"
#include "ResidualBlock.hpp"
#include "ValueHead.hpp"

struct NetworkArchitecture
{
  uint inputPlanes;            // the amount of planes (channels) the input has
  uint width;                  // the width of each plane  (=board width)
  uint height;                 // the height of each plane (=board height)
  uint residualBlocks;         // the amount of residual blocks
  uint filters;                // the amount of convolutional filters each layer
  uint policyOutputs;          // the amount of policy outputs
  uint policyFilters;          // the amount of filters in the policy layers
  uint valueFilters;           // the amount of filters in the value layers
  uint valueHeadLinearNeurons; // the amount of neurons in the value head linear layer
  // convolution options
  uint kernelSize; // the kernel size of the convolutional layers
  uint padding;    // the padding of the convolutional layers
  uint stride;     // the stride of the convolutional layers
};

/**
 * @brief The full neural network architecture based on AlphaZero
 */
struct NetworkImpl : public torch::nn::Module
{
  NetworkImpl(NetworkArchitecture const & na)
  {
    auto convBlockOptions = ConvBlockOptions{
      .inputFilters  = na.inputPlanes,
      .outputFilters = na.filters,
      .kernelSize    = na.kernelSize,
      .padding       = na.padding,
      .stride        = na.stride,
    };

    // assert that all convBlockOptions are set
    assert(convBlockOptions.inputFilters != 0);
    assert(convBlockOptions.outputFilters != 0);
    assert(convBlockOptions.kernelSize != 0);
    assert(convBlockOptions.padding != 0);
    assert(convBlockOptions.stride != 0);

    m_convInput = register_module("convInput", ConvBlock(convBlockOptions));

    // the hidden layers: a set of residual blocks
    auto residualOptions = ResidualBlockOptions{
      .inputFilters    = na.filters,
      .residualFilters = na.filters,
      .outputs         = na.filters,
      .kernelSize      = na.kernelSize,
      .padding         = na.padding,
      .stride          = na.stride,
    };

    for (uint i = 0; i < na.residualBlocks; i++)
    {
      m_resBlocks.emplace_back(register_module("resBlock" + std::to_string(i), ResidualBlock(residualOptions)));
    }

    auto valueHeadOptions = ValueHeadOptions{
      .inputFilters  = na.filters,
      .valueFilters  = na.valueFilters,
      .width         = na.width,
      .height        = na.height,
      .linearNeurons = na.valueHeadLinearNeurons,
      .outputs       = 1, // output is one value [-1, 1]
      .kernelSize    = 1, // kernel size is 1x1 for value head
      .stride        = na.stride,
    };

    auto policyHeadOptions = PolicyHeadOptions{
      .inputFilters  = na.filters,
      .policyFilters = na.policyFilters,
      .width         = na.width,
      .height        = na.height,
      .outputs       = na.policyOutputs,
      .kernelSize    = 1, // kernel size is 1x1 for policy head
      .padding       = na.padding,
      .stride        = na.stride,
    };

    m_valueHead  = register_module("valueHead", ValueHead(valueHeadOptions));
    m_policyHead = register_module("policyHead", PolicyHead(policyHeadOptions));
  }

  std::pair<torch::Tensor, torch::Tensor> forward(torch::Tensor & input)
  {
    // the first convolutional layer
    auto x = m_convInput(input);
    // all residual blocks
    for (auto & resBlock: m_resBlocks)
    {
      x = resBlock(x);
    }
    // return the two outputs
    return std::make_pair(m_policyHead(x), m_valueHead(x));
  }

private:
  ConvBlock                  m_convInput = nullptr;
  std::vector<ResidualBlock> m_resBlocks;

  PolicyHead m_policyHead = nullptr;
  ValueHead  m_valueHead  = nullptr;
};

TORCH_MODULE(Network);