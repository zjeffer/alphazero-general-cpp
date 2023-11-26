#pragma once

#include <torch/nn.h>
#include <torch/script.h>
#include <torch/torch.h>

struct PolicyHeadOptions
{
  uint inputFilters;  // The amount of input filters. Must be equal to the previous layer's output filters.
  uint policyFilters; // The amount of filters in the hidden layers.
  uint width;         // The width of each plane.
  uint height;        // The height of each plane.
  uint outputs;       // The amount of outputs in the output layer
  uint kernelSize;    // The kernel size of the convolutional layers.
  uint padding;       // The padding of the convolutional layers.
  uint stride;        // The stride of the convolutional layers.
};

/**
 * @brief The policy output architecture of the AlphaZero network
 *
 */
struct PolicyHeadImpl : public torch::nn::Module
{
  PolicyHeadImpl(PolicyHeadOptions const & options)
  {
    int policyArraySize = options.width * options.height * options.policyFilters;

    m_convPolicy = register_module(
      "convPolicy",
      torch::nn::Conv2d(torch::nn::Conv2dOptions(options.inputFilters, options.policyFilters, options.kernelSize).stride(options.stride)));
    m_batchNormPolicy = register_module("batchNormPolicy", torch::nn::BatchNorm2d(options.policyFilters));
    m_linearPolicy    = register_module("linearPolicy", torch::nn::Linear(policyArraySize, options.outputs));
  }

  /**
   * @brief Method to make a forward pass through the neural network
   *
   * @param input: the input tensor
   * @return torch::Tensor: the output tensor
   */
  torch::Tensor forward(torch::Tensor const & input)
  {
    int64_t batchSize = input.size(0);

    // conv block
    auto policyHead = m_convPolicy(input);
    policyHead      = m_batchNormPolicy(policyHead);
    policyHead      = torch::relu(policyHead);

    // flatten
    policyHead = policyHead.view({batchSize, -1});

    // linear
    policyHead = m_linearPolicy(policyHead);

    // softmax activation
    policyHead = torch::softmax(policyHead, 1);

    return policyHead;
  }

private:
  torch::nn::Conv2d      m_convPolicy      = nullptr;
  torch::nn::BatchNorm2d m_batchNormPolicy = nullptr;
  torch::nn::Linear      m_linearPolicy    = nullptr;
};
TORCH_MODULE(PolicyHead);
