#include <iostream>

#include "lib/Environment/Environment_TicTacToe.hpp"

int main()
{
  EnvironmentTicTacToe env;
  env.MakeMove(Move(0, 0));
  env.MakeMove(Move(0, 1));
  env.PrintBoard();
  return 0;
}
