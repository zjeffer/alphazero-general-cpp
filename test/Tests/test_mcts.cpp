#include <gtest/gtest.h>

#include "../Fixtures/fixture_Game.hpp"

TEST_F(GameFixture, MCTS)
{
  torch::Tensor board = torch::zeros({3, 3});
  /*
  -------------
  |   |   |   |
  -------------
  | X |   |   |
  -------------
  | X | O |   |
  -------------
  */
  board[1][0] = 1;
  board[2][0] = 1;
  board[2][1] = 2;

  environment->SetBoard(board, Player::PLAYER_2);
  // the correct move should be [0, 0]

  game.PlayMove();
  ASSERT_FALSE(environment->IsTerminal());
  ASSERT_EQ(environment->GetWinner(), Player::PLAYER_NONE);
  // ASSERT_EQ(environment->GetMoveHistory().back(), MoveTicTacToe(0, 0));
}