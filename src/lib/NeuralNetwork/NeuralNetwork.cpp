#include "NeuralNetwork.hpp"

#include "../Logging/Logger.hpp"

NeuralNetwork::NeuralNetwork()
  : m_device(torch::Device(torch::kCPU))
{
  InitializeCuda();
}

NeuralNetwork::NeuralNetwork(NetworkArchitecture const & architecture)
  : NeuralNetwork()
{
  m_net = Network(architecture);
}

NeuralNetwork::NeuralNetwork(std::filesystem::path const & path)
  : NeuralNetwork()
{
  LoadModel(path);
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

void NeuralNetwork::InitializeCuda()
{
  if (torch::cuda::is_available())
  {
    try
    {
      m_device = torch::Device(torch::kCUDA);
    }
    catch (std::exception const & e)
    {
      LWARN << "Error initializing CUDA: " << e.what();
    }
  }
  else
  {
    LWARN << "CUDA is not available. Using CPU instead.";
  }
}
