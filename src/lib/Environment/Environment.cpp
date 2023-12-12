#include "Environment.hpp"

std::ostream & operator<<(std::ostream & os, Player const & player)
{
  os << static_cast<uint8_t>(player);
  return os;
}

std::istream & operator>>(std::istream & is, Player & player)
{
  uint8_t playerInt;
  is >> playerInt;
  player = static_cast<Player>(playerInt);
  return is;
}
