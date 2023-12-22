#pragma once

#include <torch/nn.h>
#include <torch/script.h>
#include <torch/torch.h>

struct ValueHeadOptions
{
  uint inputFilters;  // The amount of input filters. Must be equal to the previous layer's output filters.
  uint valueFilters;  // The amount of filters in the hidden layers.
  uint width;         // The width of each plane.
  uint height;        // The height of each plane
  uint linearNeurons; // the amount of neurons in the linear layer
  uint outputs;       // The amount of outputs in the output layer
  // convolution options
  uint kernelSize; // The kernel size of the convolutional layers.
  uint stride;     // The stride of the convolutional layers.
};

/**
 * @brief The value output architecture of the AlphaZero network
 *
 */
struct ValueHeadImpl : public torch::nn::Module
{
  ValueHeadImpl(ValueHeadOptions const & options)
  {
    int const valueArraySize = options.width * options.height * options.valueFilters;

    m_convValue = register_module(
      "convValue",
      torch::nn::Conv2d(torch::nn::Conv2dOptions(options.inputFilters, options.valueFilters, options.kernelSize).stride(options.stride)));
    m_batchNormValue = register_module("batchNormValue", torch::nn::BatchNorm2d(options.valueFilters));

    m_linearValue1 = register_module("linearValue1", torch::nn::Linear(valueArraySize, options.linearNeurons));
    m_linearValue2 = register_module("linearValue2", torch::nn::Linear(options.linearNeurons, options.outputs));
  }

  /**
   * @brief Method to make a forward pass through the neural network
   *
   * @param input: the input tensor
   * @return torch::Tensor: the output tensor
   */
  torch::Tensor forward(torch::Tensor const & input)
  {
    int64_t size = input.size(0);

    // conv, batch norm, relu
    auto valueHead = m_convValue(input);
    valueHead      = m_batchNormValue(valueHead);
    valueHead      = torch::relu(valueHead);

    // flatten, linear, relu
    valueHead = valueHead.view({size, -1});
    valueHead = m_linearValue1(valueHead);
    valueHead = torch::relu(valueHead);

    // linear, tanh activation
    valueHead = m_linearValue2(valueHead);
    valueHead = torch::tanh(valueHead);

    return valueHead;
  }

private:
  torch::nn::Conv2d      m_convValue      = nullptr;
  torch::nn::BatchNorm2d m_batchNormValue = nullptr;
  torch::nn::Linear      m_linearValue1   = nullptr;
  torch::nn::Linear      m_linearValue2   = nullptr;
};
TORCH_MODULE(ValueHead);
