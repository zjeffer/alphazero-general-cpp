#include "Trainer.hpp"

#include "../Logging/Logger.hpp"
#include "Device.hpp"

Trainer::Trainer(std::shared_ptr<NeuralNetworkInterface> const & network)
  : m_network(network)
  , m_device(Device::GetInstance().GetDevice())
{
  m_network->GetNetwork()->to(m_device);
}

void Trainer::Train(Dataset const & dataset, TrainOptions const & trainOptions)
{
  // create data loader
  auto options    = torch::data::DataLoaderOptions().batch_size(trainOptions.batchSize).workers(4);
  auto dataLoader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(dataset, options);

  // create optimizer
  auto optimizer = torch::optim::Adam(m_network->GetNetwork()->parameters(), torch::optim::AdamOptions(trainOptions.learningRate));

  // set model to training mode
  m_network->GetNetwork()->train();

  LossHistory lossHistory;

  // run epochs
  for (size_t epoch = 0; epoch < trainOptions.epochs; ++epoch)
  {
    LINFO << "============ Epoch: " << epoch << " ============";
    // run batches
    for (auto const & batch: *dataLoader)
    {
      // move data to device
      auto data   = batch.data()->data.to(m_device);
      auto target = batch.data()->target.to(m_device);

      // zero gradients
      optimizer.zero_grad();

      // forward pass
      auto predictions = m_network->Predict(data);

      // calculate loss
      auto losses     = CalculateLoss(predictions, target);
      auto lossPolicy = losses.first;
      auto lossValue  = losses.second;
      auto loss       = lossPolicy + lossValue;

      // save loss
      lossHistory.Add(lossPolicy.item<float>(), lossValue.item<float>(), loss.item<float>());

      // backward pass
      loss.backward();

      // update parameters
      optimizer.step();

      // print loss
      // LDEBUG << "\tLoss: " << loss.item<float>() << " Policy: " << lossPolicy.item<float>() << " Value: " << lossValue.item<float>();
    }
    // loss for this epoch
    LINFO << "Average losses:"
          << " Policy: " << std::accumulate(lossHistory.policyLoss.begin(), lossHistory.policyLoss.end(), 0.0F) / lossHistory.Size()
          << ",\tValue: " << std::accumulate(lossHistory.valueLoss.begin(), lossHistory.valueLoss.end(), 0.0F) / lossHistory.Size()
          << ",\tTotal: " << std::accumulate(lossHistory.totalLoss.begin(), lossHistory.totalLoss.end(), 0.0F) / lossHistory.Size();
  }

  // set model back to evaluation mode
  m_network->GetNetwork()->eval();
}

std::pair<torch::Tensor, torch::Tensor> Trainer::CalculateLoss(std::pair<torch::Tensor, torch::Tensor> const & predictions, torch::Tensor & target)
{
  // policy target is everything exepct the last value
  auto policyTarget = target.slice(1, 0, target.size(1) - 1);
  // value target is the last value
  auto valueTarget = target.slice(1, target.size(1) - 1, target.size(1));

  // calculated using the cross entropy loss
  auto policyLoss = -torch::log_softmax(predictions.first, 1).mul(policyTarget).sum(1).mean();
  // calculated using the mean squared error loss
  auto valueLoss = torch::nn::functional::mse_loss(predictions.second, valueTarget);

  valueLoss *= 9.0F;

  return {policyLoss, valueLoss};
}
