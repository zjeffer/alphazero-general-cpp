#include "../Logging/Logger.hpp"
#include "Configuration.hpp"

template<typename T>
T Configuration::Get(std::string const & key)
{
  try
  {
    if (key.empty())
    {
      throw std::runtime_error("Cannot get Json value with empty json key");
    }
    auto pointer = nlohmann::json::json_pointer(key.starts_with("/") ? key : "/" + key);
    return m_root[pointer].get<T>();
  }
  catch (std::exception const & e)
  {
    LWARN << "Failed to get configuration value: " << e.what();
    throw std::runtime_error("Failed to get configuration value for key: " + key);
  }
}

template<typename T>
void Configuration::Set(std::string const & key, T const & value)
{
  try
  {
    auto pointer    = nlohmann::json::json_pointer(key.starts_with("/") ? key : "/" + key);
    m_root[pointer] = value;
  }
  catch (std::exception const & e)
  {
    LWARN << "Failed to set configuration value: " << e.what();
    throw std::runtime_error("Failed to set configuration value for key: " + key + " with value: " + std::to_string(value));
  }
}