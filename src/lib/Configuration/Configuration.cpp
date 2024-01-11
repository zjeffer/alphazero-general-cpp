#include "Configuration.hpp"

#include <fstream>

#include "../Logging/Logger.hpp"

using json = nlohmann::json;

Configuration::Configuration(std::filesystem::path const & filePath)
  : m_filePath(filePath)
{

  try
  {
    if (!std::filesystem::exists(filePath))
    {
      std::ofstream file(filePath);
      file << "{}";
    }
    std::ifstream file(filePath);
    // parse with nlohmann_json (no callback, don't allow exceptions, allow comments)
    m_root = json::parse(file, nullptr, false, true);
  }
  catch (std::exception const & e)
  {
    throw std::runtime_error("Failed to read the configuration file: " + std::string(e.what()));
  }
}

void Configuration::Save()
{
  LDEBUG << "Saving configuration to " << m_filePath;
  try
  {
    std::ofstream file(m_filePath);
    file << m_root;
  }
  catch (std::exception const & e)
  {
    throw std::runtime_error("Failed to save the configuration file: " + std::string(e.what()));
  }
}