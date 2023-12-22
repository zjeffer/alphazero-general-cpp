#include "NeuralNetwork.hpp"

#include "../Logging/Logger.hpp"

NeuralNetwork::NeuralNetwork()
  : m_device(Device())
{
}

NeuralNetwork::NeuralNetwork(NetworkArchitecture const & architecture)
  : NeuralNetwork()
{
  m_net = Network(architecture);
  m_net->to(m_device.GetDevice());
}

NeuralNetwork::NeuralNetwork(NetworkArchitecture const & architecture, std::filesystem::path const & path)
  : NeuralNetwork(architecture)
{
  LoadModel(path);
}

Network NeuralNetwork::GetNetwork()
{
  return m_net;
}

std::pair<torch::Tensor, torch::Tensor> NeuralNetwork::Predict(torch::Tensor & input)
{
  try
  {
    return m_net->forward(input);
  }
  catch (std::exception const & e)
  {
    LWARN << "Error in network Predict function: " << e.what();
    throw std::runtime_error("Error in network Predict function: " + std::string(e.what()));
  }
}

bool NeuralNetwork::LoadModel(std::filesystem::path const & path)
{
  LINFO << "Loading model from " << path;
  try
  {
    torch::load(m_net, path, m_device.GetDevice());
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
