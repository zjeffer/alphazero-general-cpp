#pragma once

#include <torch/nn.h>
#include <torch/script.h>
#include <torch/torch.h>

struct ResidualBlockOptions
{
  uint inputFilters;    // The amount of input filters. Must be equal to the previous layer's output filters.
  uint residualFilters; // The amount of filters in the hidden layers.
  uint outputs;         // The amount of outputs in the output layer
  // convolution options
  uint kernelSize; // The kernel size of the convolutional layers.
  uint padding;    // The padding of the convolutional layers.
  uint stride;     // The stride of the convolutional layers.
};

/**
 * @brief A residual block consists of multiple convolutional layers with skip connections.
 *
 */
struct ResidualBlockImpl : public torch::nn::Module
{
  ResidualBlockImpl(ResidualBlockOptions const & options)
  {
    register_module(
      "conv1",
      conv1 = torch::nn::Conv2d(
        torch::nn::Conv2dOptions(options.inputFilters, options.residualFilters, options.kernelSize).padding(options.padding).stride(options.stride)));
    register_module("batchNorm1", batchNorm1 = torch::nn::BatchNorm2d(options.residualFilters));

    register_module("conv2",
                    conv2 = torch::nn::Conv2d(
                      torch::nn::Conv2dOptions(options.residualFilters, options.outputs, options.kernelSize).padding(options.padding).stride(options.stride)));
    register_module("batchNorm2", batchNorm2 = torch::nn::BatchNorm2d(options.outputs));
  }

  /**
   * @brief Method to make a forward pass through the neural network
   *
   * @param input: the input tensor
   * @return torch::Tensor: the output tensor
   */
  torch::Tensor forward(torch::Tensor const & input)
  {
    torch::Tensor x = input;
    // first conv block
    x = batchNorm1(conv1(input));
    x = torch::relu(x);
    // second conv block
    x = batchNorm2(conv2(x));
    // skip connection, then relu
    x = torch::relu(x + input);
    return x;
  }

  torch::nn::Conv2d      conv1 = nullptr, conv2 = nullptr;
  torch::nn::BatchNorm2d batchNorm1 = nullptr, batchNorm2 = nullptr;
};
TORCH_MODULE(ResidualBlock);