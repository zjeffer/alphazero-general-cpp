#include "NeuralNetwork.hpp"

#include "../Logging/Logger.hpp"

NeuralNetwork::NeuralNetwork(NetworkArchitecture const & architecture)
  : m_device(torch::Device(torch::kCPU))
{
  if (torch::cuda::is_available())
  {
    m_device = torch::Device(torch::kCUDA);
  }
  m_net = Network(architecture);
}

Network NeuralNetwork::GetNetwork()
{
  return m_net;
}

std::pair<torch::Tensor, torch::Tensor> NeuralNetwork::Predict(torch::Tensor & input)
{
  return m_net->forward(input);
}

bool NeuralNetwork::LoadModel(std::filesystem::path const & path)
{
  try
  {
    torch::load(m_net, path);
  }
  catch (std::exception const & e)
  {
    LWARN << "Error loading model: " << e.what();
    return false;
  }
  return true;
}

std::filesystem::path NeuralNetwork::SaveModel(std::filesystem::path const & path)
{
  try
  {
    torch::save(m_net, path);
  }
  catch (std::exception const & e)
  {
    LWARN << "Error saving model: " << e.what();
    throw;
  }
  return path;
}
