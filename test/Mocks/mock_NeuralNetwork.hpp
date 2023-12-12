#pragma once

#include <gmock/gmock.h>

#include "../../src/lib/NeuralNetwork/NeuralNetworkInterface.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

namespace fs = std::filesystem;

class NeuralNetworkMock : public NeuralNetworkInterface
{
private:
  Network m_network = nullptr;

public:
  NeuralNetworkMock()
  {
    ON_CALL(*this, GetNetwork()).WillByDefault(Return(m_network));
    ON_CALL(*this, Predict(_)).WillByDefault(Invoke([](torch::Tensor & input) { //
      auto policySize = input.size(1) * input.size(2);
      auto policy     = torch::ones({1, policySize});
      auto value      = torch::ones({1, 1});
      return std::make_pair(policy, value);
    }));
    ON_CALL(*this, LoadModel(_)).WillByDefault(Return(true));
    ON_CALL(*this, SaveModel(_)).WillByDefault(Invoke([](fs::path const & path) { return path; }));

    EXPECT_CALL(*this, Predict(_)).Times(testing::AnyNumber());
  }

  MOCK_METHOD(Network, GetNetwork, (), (override));
  MOCK_METHOD((std::pair<torch::Tensor, torch::Tensor>), Predict, (torch::Tensor & input), (override));
  MOCK_METHOD(bool, LoadModel, (fs::path const & path), (override));
  MOCK_METHOD(fs::path, SaveModel, (fs::path const & path), (override));
};
