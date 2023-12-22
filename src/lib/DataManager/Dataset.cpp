#include "Dataset.hpp"

Dataset::Dataset(std::vector<MemoryElement> const & memoryElements)
{
  try
  {
    for (auto const & memoryElement: memoryElements)
    {
      auto [input, target] = memoryElement.ConvertToInputAndOutput();
      m_data.emplace_back(std::move(input), std::move(target));
    }
  }
  catch (std::exception const & e)
  {
    throw std::runtime_error("Failed to create dataset from memory elements: " + std::string(e.what()));
  }
}

torch::data::Example<> Dataset::get(size_t index)
{
  auto [input, target] = m_data[index];
  return {std::move(input), std::move(target)};
}

torch::optional<size_t> Dataset::size() const
{
  return m_data.size();
}
