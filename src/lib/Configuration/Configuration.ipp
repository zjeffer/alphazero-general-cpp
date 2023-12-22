#include "../Logging/Logger.hpp"
#include "Configuration.hpp"

template<typename T>
T Configuration::Get(std::string const & key)
{
  // key can be "string" or "string.string.string"
  auto value = m_root;
  // split the key into a vector of strings
  std::vector<std::string> keyParts;
  std::string              keyPart;
  std::stringstream        keyStream(key);
  while (std::getline(keyStream, keyPart, '.'))
  {
    keyParts.push_back(keyPart);
  }
  // get the value
  try
  {
    for (auto const & keyPart: keyParts)
    {
      if (value.is_object() || value.is_array())
      {
        value = value[keyPart];
      }
      else
      {
        LWARN << "Configuration value is not an object or an array.";
      }
    }
  }
  catch (std::exception const & e)
  {
    throw std::runtime_error("Failed to get configuration value: " + std::string(e.what()));
  }
  try
  {
    // return the value as the requested type using nlohmann-json
    return value.get<T>();
  }
  catch (std::exception const & e)
  {
    throw std::runtime_error("Failed to convert configuration value: " + std::string(e.what()));
  }
}

template<typename T>
void Configuration::Set(std::string const & key, T const & value)
{
  m_root[key] = value;
}