#pragma once

#include <filesystem>
#include <tuple>

#include "../../Configuration/Configuration.hpp"
#include "ConvBlock.hpp"
#include "PolicyHead.hpp"
#include "ResidualBlock.hpp"
#include "ValueHead.hpp"

struct NetworkArchitecture
{
  uint width;                  // the width of each plane  (=board width)
  uint height;                 // the height of each plane (=board height)
  uint inputPlanes;            // the amount of planes (channels) the input has
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

  NetworkArchitecture() = default;
  NetworkArchitecture(std::filesystem::path const & file)
  {
    auto config            = Configuration(file);
    width                  = config.Get<uint>("board/width");
    height                 = config.Get<uint>("board/height");
    inputPlanes            = config.Get<uint>("network_architecture/input_planes");
    residualBlocks         = config.Get<uint>("network_architecture/residual_blocks");
    filters                = config.Get<uint>("network_architecture/filters");
    policyOutputs          = config.Get<uint>("network_architecture/policy_outputs");
    policyFilters          = config.Get<uint>("network_architecture/policy_filters");
    valueFilters           = config.Get<uint>("network_architecture/value_filters");
    valueHeadLinearNeurons = config.Get<uint>("network_architecture/value_head_linear_neurons");
    kernelSize             = config.Get<uint>("network_architecture/kernel/size");
    padding                = config.Get<uint>("network_architecture/kernel/padding");
    stride                 = config.Get<uint>("network_architecture/kernel/stride");
  }

  void SaveToFile(std::filesystem::path const & file) const
  {
    Configuration config = Configuration(file);
    config.Set<uint>("board/width", width);
    config.Set<uint>("board/height", height);
    config.Set<uint>("network_architecture/input_planes", inputPlanes);
    config.Set<uint>("network_architecture/residual_blocks", residualBlocks);
    config.Set<uint>("network_architecture/filters", filters);
    config.Set<uint>("network_architecture/policy_outputs", policyOutputs);
    config.Set<uint>("network_architecture/policy_filters", policyFilters);
    config.Set<uint>("network_architecture/value_filters", valueFilters);
    config.Set<uint>("network_architecture/value_head_linear_neurons", valueHeadLinearNeurons);
    config.Set<uint>("network_architecture/kernel/size", kernelSize);
    config.Set<uint>("network_architecture/kernel/padding", padding);
    config.Set<uint>("network_architecture/kernel/stride", stride);
    config.Save();
  }
};

/**
 * @brief The full neural network architecture based on AlphaZero
 */
struct NetworkImpl : public torch::nn::Module
{
  NetworkImpl() = default; // necessary for gmock.

  NetworkImpl(NetworkArchitecture const & na)
  {
    auto convBlockOptions = ConvBlockOptions{
      .inputFilters  = na.inputPlanes,
      .outputFilters = na.filters,
      .kernelSize    = na.kernelSize,
      .padding       = na.padding,
      .stride        = na.stride,
    };

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

  std::pair<torch::Tensor, torch::Tensor> forward(torch::Tensor & input) // NOLINT(readability-identifier-naming)
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