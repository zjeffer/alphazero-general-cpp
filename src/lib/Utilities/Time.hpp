#pragma once

#include <chrono>
#include <string>

inline std::string GetTimeAsString(std::string const & format, bool addMilliseconds = false)
{
  if (format.empty())
  {
    throw std::runtime_error("Format string cannot be empty");
  }
  auto const        now     = std::chrono::system_clock::now();
  auto const        nowTime = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&nowTime), format.c_str());
  if (addMilliseconds)
  {
    auto const ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
  }
  return ss.str();
}