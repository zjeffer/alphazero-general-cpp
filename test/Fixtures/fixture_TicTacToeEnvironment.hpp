#pragma once

#include <gtest/gtest.h>

#include "../../src/lib/Environment/Environment_TicTacToe.hpp"

struct EnvironmentTicTacToeFixture : public ::testing::Test
{
  EnvironmentTicTacToeFixture()
    : env()
  {
  }
  ~EnvironmentTicTacToeFixture() override = default;

  EnvironmentTicTacToe env;
};

struct EnvironmentTicTacToeFixtureRandomMovesPlayed : EnvironmentTicTacToeFixture
{
  EnvironmentTicTacToeFixtureRandomMovesPlayed()
  {
    env.MakeMove(MoveTicTacToe(0, 0));
    env.MakeMove(MoveTicTacToe(0, 1));
    env.MakeMove(MoveTicTacToe(2, 1));
    env.MakeMove(MoveTicTacToe(2, 2));
    /*
    Board:
    *  -------------
    *  | X | O |   |
    *  -------------
    *  |   |   |   |
    *  -------------
    *  |   | X | O |
    *  -------------
    */
  }
};

struct EnvironmentTicTacToeFixturePlayerXHasWon : EnvironmentTicTacToeFixture
{
  EnvironmentTicTacToeFixturePlayerXHasWon()
  {
    env.MakeMove(MoveTicTacToe(0, 0));
    env.MakeMove(MoveTicTacToe(1, 0));
    env.MakeMove(MoveTicTacToe(0, 1));
    env.MakeMove(MoveTicTacToe(1, 1));
    env.MakeMove(MoveTicTacToe(0, 2));
    /* Board:
     *  -------------
     *  | X | X | X |
     *  -------------
     *  | O | O |   |
     *  -------------
     *  |   |   |   |
     *  -------------
     */
  }
};

struct EnvironmentTicTacToeFixtureBoardFull : EnvironmentTicTacToeFixture
{
  EnvironmentTicTacToeFixtureBoardFull()
  {
    env.MakeMove(MoveTicTacToe(0, 0));
    env.MakeMove(MoveTicTacToe(0, 1));
    env.MakeMove(MoveTicTacToe(0, 2));
    env.MakeMove(MoveTicTacToe(1, 0));
    env.MakeMove(MoveTicTacToe(1, 1));
    env.MakeMove(MoveTicTacToe(1, 2));
    env.MakeMove(MoveTicTacToe(2, 0));
    env.MakeMove(MoveTicTacToe(2, 1));
    env.MakeMove(MoveTicTacToe(2, 2));
    /* Board:
     *  -------------
     *  | X | X | X |
     *  -------------
     *  | X | X | X |
     *  -------------
     *  | X | X | X |
     *  -------------
     */
  }
};