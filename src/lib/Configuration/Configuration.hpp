#pragma once

#include <filesystem>

#include <nlohmann/json.hpp>

class Configuration
{
private:
  std::filesystem::path m_filePath;
  nlohmann::json        m_root;

public:
  Configuration(std::filesystem::path const & filePath);
  ~Configuration() = default;

  template<typename T>
  T Get(std::string const & key);

  template<typename T>
  void Set(std::string const & key, T const & value);

  void Save();
};

#include "Configuration.ipp"