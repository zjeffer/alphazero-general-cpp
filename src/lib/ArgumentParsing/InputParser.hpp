#pragma once

#include <algorithm>
#include <string>
#include <vector>

// argument parsing (https://stackoverflow.com/a/868894/10180569)
class InputParser
{
public:
  InputParser(int & argc, char ** argv)
  {
    for (int i = 1; i < argc; ++i)
    {
      m_tokens.emplace_back(argv[i]);
    }
  }

  /// @author iain
  std::string const & GetCmdOption(std::string const & option) const
  {
    std::vector<std::string>::const_iterator itr;
    itr = std::find(m_tokens.begin(), m_tokens.end(), option);
    if (itr != m_tokens.end() && ++itr != m_tokens.end())
    {
      return *itr;
    }
    static const std::string emptyString("");
    return emptyString;
  }

  /// @author iain
  bool CmdOptionExists(std::string const & option) const
  {
    return std::find(m_tokens.begin(), m_tokens.end(), option) != m_tokens.end();
  }

private:
  std::vector<std::string> m_tokens;
};