#pragma once

#include <torch/torch.h>

#include "MemoryElement.hpp"

using Data = std::pair<torch::Tensor, torch::Tensor>;

class Dataset : public torch::data::datasets::Dataset<Dataset>
{
public:
  explicit Dataset(std::vector<MemoryElement> const & memoryElements);

  torch::data::Example<>  get(size_t index) override;
  torch::optional<size_t> size() const override;

private:
  std::vector<Data> m_data;
};