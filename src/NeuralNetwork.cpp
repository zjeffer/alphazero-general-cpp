#include "NeuralNetwork.hpp"

NeuralNetwork::NeuralNetwork()
  : m_device(torch::Device(torch::kCPU))
{
  if (torch::cuda::is_available())
  {
    m_device = torch::Device(torch::kCUDA);
  }
}

Network NeuralNetwork::GetNetwork()
{
  return m_net;
}

torch::Tensor NeuralNetwork::BoardToInput(torch::Tensor const & board, Player player, int inputPlanes) {}

torch::Tensor NeuralNetwork::BoardToInput(std::shared_ptr<Environment> const & env) {}

std::pair<torch::Tensor, torch::Tensor> NeuralNetwork::Predict(torch::Tensor const & input) {}

bool NeuralNetwork::LoadModel(std::filesystem::path const & path) {}

std::filesystem::path NeuralNetwork::SaveModel(std::filesystem::path const & path) {}
