#include <gtest/gtest.h>

#include "../Fixtures/fixture_Game.hpp"
#include "../Fixtures/fixture_MCTS.hpp"

TEST_F(GameFixture, MCTS_XWinning_XTurn_XShouldWin)
{
  torch::Tensor board = torch::zeros({3, 3});
  /*
  -------------
  |   |   | O |
  -------------
  | X |   |   |
  -------------
  | X | O |   |
  -------------
  */
  board[0][2] = 2;
  board[1][0] = 1;
  board[2][0] = 1;
  board[2][1] = 2;

  environment->SetBoard(board, Player::PLAYER_1);

  game.PlayMove();
  ASSERT_TRUE(environment->IsTerminal());
  ASSERT_TRUE(environment->GetWinner() == Player::PLAYER_1);
}

TEST_F(GameFixture, MCTS_OWinning_OTurn_OShouldWin)
{
  torch::Tensor board = torch::zeros({3, 3});
  /*
  -------------
  |   |   | X |
  -------------
  | O |   |   |
  -------------
  | O | X |   |
  -------------
  */
  board[0][2] = 1;
  board[1][0] = 2;
  board[2][0] = 2;
  board[2][1] = 1;

  environment->SetBoard(board, Player::PLAYER_2);

  game.PlayMove();
  ASSERT_TRUE(environment->IsTerminal());
  ASSERT_TRUE(environment->GetWinner() == Player::PLAYER_2);
}

TEST_F(GameFixture, MCTS_XWinning_OTurn_ShouldEndInDraw)
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

  game.PlayMove();
  ASSERT_FALSE(environment->IsTerminal());
  ASSERT_EQ(environment->GetWinner(), Player::PLAYER_NONE);
}

TEST_F(GameFixture, MCTS_OWinning_XTurn_ShouldEndInDraw)
{
  torch::Tensor board = torch::zeros({3, 3});
  /*
  -------------
  |   |   | X |
  -------------
  | O |   |   |
  -------------
  | O | X |   |
  -------------
  */
  board[0][2] = 1;
  board[1][0] = 2;
  board[2][0] = 2;
  board[2][1] = 1;

  environment->SetBoard(board, Player::PLAYER_1);

  game.PlayMove();
  ASSERT_FALSE(environment->IsTerminal());
  ASSERT_EQ(environment->GetWinner(), Player::PLAYER_NONE);
}

TEST_F(GameFixture, MCTS_Draw_XTurn)
{
  torch::Tensor board = torch::zeros({3, 3});
  /*
  -------------
  | X | O |   |
  -------------
  | O | O | X |
  -------------
  | X | X | O |
  -------------
  */
  board[0][0] = 1;
  board[0][1] = 2;

  board[1][0] = 2;
  board[1][1] = 2;
  board[1][2] = 1;

  board[2][0] = 1;
  board[2][1] = 1;
  board[2][2] = 2;

  environment->SetBoard(board, Player::PLAYER_1);

  game.PlayMove();
  ASSERT_TRUE(environment->IsTerminal());
  ASSERT_EQ(environment->GetWinner(), Player::PLAYER_NONE);
}

TEST_F(GameFixture, MCTS_Draw_OTurn)
{
  torch::Tensor board = torch::zeros({3, 3});
  /*
  -------------
  | X | O |   |
  -------------
  | O | O | X |
  -------------
  | X | X | O |
  -------------
  */
  board[0][0] = 1;
  board[0][1] = 2;

  board[1][0] = 2;
  board[1][1] = 2;
  board[1][2] = 1;

  board[2][0] = 1;
  board[2][1] = 1;
  board[2][2] = 2;

  environment->SetBoard(board, Player::PLAYER_2);

  game.PlayMove();
  ASSERT_TRUE(environment->IsTerminal());
  ASSERT_EQ(environment->GetWinner(), Player::PLAYER_NONE);
}

TEST_F(MCTSFixture, MCTS_GetBestMove_Deterministic)
{
  torch::Tensor board = torch::zeros({3, 3});
  /*
  -------------
  |   |   | O |
  -------------
  | X |   |   |
  -------------
  | X | O |   |
  -------------
  */
  board[0][2] = 2;
  board[1][0] = 1;
  board[2][0] = 1;
  board[2][1] = 2;

  env->SetBoard(board, Player::PLAYER_1);
  NeuralNetworkMock network;
  mcts.RunSimulations(200, network);
  auto bestMove = mcts.GetBestMove(false);
  ASSERT_EQ(bestMove->GetRow(), 0);
  ASSERT_EQ(bestMove->GetColumn(), 0);
}