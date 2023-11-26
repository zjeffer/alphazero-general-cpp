#pragma once

#include <torch/nn.h>
#include <torch/script.h>
#include <torch/torch.h>

struct ConvBlockOptions
{
  uint inputFilters;  // The amount of input filters. Must be equal to the previous layer's output filters.
  uint outputFilters; // The amount of filters in the hidden layers.
  uint kernelSize;    // The kernel size of the convolutional layers.
  uint padding;       // The padding of the convolutional layers.
  uint stride;        // The stride of the convolutional layers.
};

/**
 * @brief A set of layers reprenting a convolutional block
 *
 */
struct ConvBlockImpl : public torch::nn::Module
{
  /**
   * @brief Construct a new convolutional block
   *
   * @param input_filters: The amount of input filters
   * @param output_filters: The amount of output filters
   */
  ConvBlockImpl(ConvBlockOptions const & options)
  {
    m_conv1 = register_module(
      "conv1",
      torch::nn::Conv2d(
        torch::nn::Conv2dOptions(options.inputFilters, options.outputFilters, options.kernelSize).padding(options.padding).stride(options.stride)));

    m_batchNorm1 = register_module("batchNorm1", torch::nn::BatchNorm2d(options.outputFilters));
  }

  /**
   * @brief Method to make a forward pass through the neural network
   *
   * @param x: the input tensor
   * @return torch::Tensor: the output tensor
   */
  torch::Tensor forward(torch::Tensor const & x)
  {
    // convolutional layer, then batch normalisation, then ReLU
    return torch::relu(m_batchNorm1(m_conv1(x)));
  }

private:
  torch::nn::Conv2d      m_conv1      = nullptr;
  torch::nn::BatchNorm2d m_batchNorm1 = nullptr;
};
TORCH_MODULE(ConvBlock);
