#include "NeuralNetwork.hpp"

#include "../Logging/Logger.hpp"

NeuralNetwork::NeuralNetwork()
  : m_device(Device::GetInstance())
{
}

NeuralNetwork::NeuralNetwork(NetworkArchitecture const & architecture)
  : NeuralNetwork()
{
  // create a new network with the given architecture
  m_architecture = architecture;
  m_net          = Network(architecture);
  m_net->to(m_device.GetDevice());
}

NeuralNetwork::NeuralNetwork(std::filesystem::path const & folder)
  : NeuralNetwork()
{
  LoadModel(folder);
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

bool NeuralNetwork::LoadModel(std::filesystem::path const & folder)
{
  LINFO << "Loading model from " << folder;
  try
  {
    m_architecture = NetworkArchitecture(folder / "model.jsonc");
    torch::load(m_net, folder / "model.pt", m_device.GetDevice());
    m_net->to(m_device.GetDevice());
  }
  catch (std::exception const & e)
  {
    LWARN << "Error loading model: " << e.what();
    return false;
  }
  return true;
}

std::filesystem::path NeuralNetwork::SaveModel(std::filesystem::path const & folder)
{
  LINFO << "Saving model to " << folder;
  try
  {
    std::filesystem::create_directories(folder);
    m_architecture.SaveToFile(folder / "model.jsonc");
    torch::save(m_net, folder / "model.pt");
  }
  catch (std::exception const & e)
  {
    LWARN << "Error saving model: " << e.what();
    throw;
  }
  return folder;
}
