#pragma once

#include "../DataManager/Dataset.hpp"
#include "NeuralNetworkInterface.hpp"

using DataLoader
  = torch::data::StatelessDataLoader<torch::data::datasets::MapDataset<Dataset, torch::data::transforms::Stack<torch::data::Example<at::Tensor, at::Tensor>>>,
                                     torch::data::samplers::RandomSampler>;

struct TrainOptions
{
  size_t batchSize    = 32;
  size_t epochs       = 10;
  float  learningRate = 0.001F;

  TrainOptions(std::filesystem::path const & file)
  {
    auto config  = Configuration(file);
    batchSize    = config.Get<uint>("batch_size");
    epochs       = config.Get<uint>("epochs");
    learningRate = config.Get<float>("learning_rate");
  }
};

class Trainer
{
private:
  std::shared_ptr<NeuralNetworkInterface> m_network;
  torch::Device                           m_device;

public:
  Trainer(std::shared_ptr<NeuralNetworkInterface> const & network);
  ~Trainer() = default;

  void Train(Dataset const & dataset, TrainOptions const & trainOptions);

private:
  std::pair<torch::Tensor, torch::Tensor> CalculateLoss(std::pair<torch::Tensor, torch::Tensor> const & predictions, torch::Tensor & target);
};

struct LossHistory
{
  std::vector<float> policyLoss;
  std::vector<float> valueLoss;
  std::vector<float> totalLoss;

  void Add(float policy, float value, float total)
  {
    policyLoss.push_back(policy);
    valueLoss.push_back(value);
    totalLoss.push_back(total);
  }

  void Save(std::filesystem::path const & path)
  {
    std::ofstream file(path);

    for (size_t i = 0; i < policyLoss.size(); ++i)
    {
      file << policyLoss[i] << "," << valueLoss[i] << "," << totalLoss[i] << std::endl;
    }
  }

  void Clear()
  {
    policyLoss.clear();
    valueLoss.clear();
    totalLoss.clear();
  }

  size_t Size() const
  {
    return totalLoss.size();
  }

  float AveragePolicyLoss() const
  {
    return std::accumulate(policyLoss.begin(), policyLoss.end(), 0.0F) / policyLoss.size();
  }

  float AverageValueLoss() const
  {
    return std::accumulate(valueLoss.begin(), valueLoss.end(), 0.0F) / valueLoss.size();
  }

  float AverageTotalLoss() const
  {
    return std::accumulate(totalLoss.begin(), totalLoss.end(), 0.0F) / totalLoss.size();
  }
};