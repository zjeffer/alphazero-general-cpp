
#include "../../src/lib/Logging/Logger.hpp"
#include "../Fixtures/fixture_TicTacToeEnvironment.hpp"

TEST_F(EnvironmentTicTacToeFixtureRandomMovesPlayed, Clone)
{
  auto clone = env.Clone();

  ASSERT_EQ(env.GetBoard().sizes(), clone->GetBoard().sizes());
  ASSERT_TRUE(torch::equal(env.GetBoard(), clone->GetBoard()));
  ASSERT_EQ(env.GetMoveHistory().size(), clone->GetMoveHistory().size());
  ASSERT_EQ(env.GetCurrentPlayer(), clone->GetCurrentPlayer());
}

TEST_F(EnvironmentTicTacToeFixtureRandomMovesPlayed, Clone_CheckMoveHistory)
{
  auto clone = env.Clone();

  ASSERT_EQ(env.GetMoveHistory().size(), clone->GetMoveHistory().size());
  for (int i = 0; i < env.GetMoveHistory().size(); i++)
  {
    ASSERT_EQ(env.GetMoveHistory()[i]->GetRow(), clone->GetMoveHistory()[i]->GetRow());
    ASSERT_EQ(env.GetMoveHistory()[i]->GetColumn(), clone->GetMoveHistory()[i]->GetColumn());
  }
}

TEST_F(EnvironmentTicTacToeFixtureRandomMovesPlayed, Clone_CheckCurrentPlayer)
{
  auto clone = env.Clone();

  ASSERT_EQ(env.GetCurrentPlayer(), clone->GetCurrentPlayer());
}

TEST_F(EnvironmentTicTacToeFixture, GetCurrentPlayer)
{
  ASSERT_EQ(env.GetCurrentPlayer(), Player::PLAYER_1);
}

TEST_F(EnvironmentTicTacToeFixture, SetCurrentPlayer)
{
  ASSERT_EQ(env.GetCurrentPlayer(), Player::PLAYER_1);
  env.SetCurrentPlayer(Player::PLAYER_2);
  ASSERT_EQ(env.GetCurrentPlayer(), Player::PLAYER_2);
}

TEST_F(EnvironmentTicTacToeFixture, TogglePlayer)
{
  env.TogglePlayer();
  ASSERT_EQ(env.GetCurrentPlayer(), Player::PLAYER_2);
  env.TogglePlayer();
  ASSERT_EQ(env.GetCurrentPlayer(), Player::PLAYER_1);
}

TEST_F(EnvironmentTicTacToeFixture, MakeMove)
{
  env.MakeMove(MoveTicTacToe(0, 0));
  ASSERT_EQ(env.GetPlayerAtCoordinates(0, 0), Player::PLAYER_1);
  env.MakeMove(MoveTicTacToe(1, 1));
  ASSERT_EQ(env.GetPlayerAtCoordinates(1, 1), Player::PLAYER_2);
}

TEST_F(EnvironmentTicTacToeFixture, MakeMove_InvalidMove_RepeatMove)
{
  env.MakeMove(MoveTicTacToe(0, 0));
  // make the same move again
  ASSERT_THROW(env.MakeMove(MoveTicTacToe(0, 0)), std::runtime_error);
}

TEST_F(EnvironmentTicTacToeFixture, MakeMove_InvalidMove_BoardFull)
{
  env.MakeMove(MoveTicTacToe(0, 0));
  ASSERT_THROW(env.MakeMove(MoveTicTacToe(0, 0)), std::runtime_error);
}

TEST_F(EnvironmentTicTacToeFixture, UndoMove)
{
  env.MakeMove(MoveTicTacToe(0, 0));
  env.MakeMove(MoveTicTacToe(1, 1));
  env.UndoMove();
  ASSERT_EQ(env.GetPlayerAtCoordinates(1, 1), Player::PLAYER_NONE);
  env.UndoMove();
  ASSERT_EQ(env.GetPlayerAtCoordinates(0, 0), Player::PLAYER_NONE);
}

TEST_F(EnvironmentTicTacToeFixture, IsValidMove)
{
  ASSERT_TRUE(env.IsValidMove(0, 0));
  env.MakeMove(MoveTicTacToe(0, 0));
  ASSERT_FALSE(env.IsValidMove(0, 0));
}

TEST_F(EnvironmentTicTacToeFixture, GetValidMoves)
{
  auto validMoves = env.GetValidMoves();
  ASSERT_EQ(validMoves.size(), 9);
  env.MakeMove(MoveTicTacToe(0, 0));
  validMoves = env.GetValidMoves();
  ASSERT_EQ(validMoves.size(), 8);
}

TEST_F(EnvironmentTicTacToeFixture, GetRows)
{
  ASSERT_EQ(env.GetRows(), 3);
}

TEST_F(EnvironmentTicTacToeFixture, GetColumns)
{
  ASSERT_EQ(env.GetColumns(), 3);
}

TEST_F(EnvironmentTicTacToeFixture, GetPlayerAtCoordinates)
{
  ASSERT_EQ(env.GetPlayerAtCoordinates(0, 0), Player::PLAYER_NONE);
  env.MakeMove(MoveTicTacToe(0, 0));
  ASSERT_EQ(env.GetPlayerAtCoordinates(0, 0), Player::PLAYER_1);
}

TEST_F(EnvironmentTicTacToeFixture, GetBoard)
{
  ASSERT_EQ(env.GetBoard().sizes(), torch::IntArrayRef({3, 3}));
}

TEST_F(EnvironmentTicTacToeFixture, SetBoard)
{
  auto board = torch::zeros({3, 3});
  env.SetBoard(board, Player::PLAYER_2);
  ASSERT_TRUE(torch::allclose(env.GetBoard(), board));
  ASSERT_EQ(env.GetCurrentPlayer(), Player::PLAYER_2);
}

TEST_F(EnvironmentTicTacToeFixture, BoardToInput)
{
  auto input = env.BoardToInput();
  ASSERT_EQ(input.sizes(), torch::IntArrayRef({1, 3, 3, 3}));
}

TEST_F(EnvironmentTicTacToeFixture, IsTerminal)
{
  ASSERT_FALSE(env.IsTerminal());
  env.MakeMove(MoveTicTacToe(0, 0));
  env.MakeMove(MoveTicTacToe(1, 0));
  env.MakeMove(MoveTicTacToe(0, 1));
  env.MakeMove(MoveTicTacToe(1, 1));
  env.MakeMove(MoveTicTacToe(0, 2));
  env.MakeMove(MoveTicTacToe(1, 2));
  env.PrintBoard();
  ASSERT_TRUE(env.IsTerminal());
}

TEST_F(EnvironmentTicTacToeFixture, GetWinner_None)
{
  ASSERT_EQ(env.GetWinner(), Player::PLAYER_NONE);
}

TEST_F(EnvironmentTicTacToeFixturePlayerXHasWon, GetWinner_PlayerXFixture)
{
  ASSERT_EQ(env.GetWinner(), Player::PLAYER_1);
}

TEST_F(EnvironmentTicTacToeFixture, GetWinner_Horizontal)
{
  env.MakeMove(MoveTicTacToe(0, 0));
  env.MakeMove(MoveTicTacToe(1, 0));
  env.MakeMove(MoveTicTacToe(0, 1));
  env.MakeMove(MoveTicTacToe(1, 1));
  env.MakeMove(MoveTicTacToe(0, 2));
  ASSERT_EQ(env.GetWinner(), Player::PLAYER_1);
}

TEST_F(EnvironmentTicTacToeFixture, GetWinner_Vertical)
{
  env.MakeMove(MoveTicTacToe(0, 1));
  env.MakeMove(MoveTicTacToe(0, 0));
  env.MakeMove(MoveTicTacToe(1, 1));
  env.MakeMove(MoveTicTacToe(1, 0));
  env.MakeMove(MoveTicTacToe(2, 1));
  env.PrintBoard();
  ASSERT_EQ(env.GetWinner(), Player::PLAYER_1);
}

TEST_F(EnvironmentTicTacToeFixture, GetWinner_Diagonal)
{
  env.MakeMove(MoveTicTacToe(0, 0));
  env.MakeMove(MoveTicTacToe(0, 1));
  env.MakeMove(MoveTicTacToe(1, 1));
  env.MakeMove(MoveTicTacToe(0, 2));
  env.MakeMove(MoveTicTacToe(2, 2));
  ASSERT_EQ(env.GetWinner(), Player::PLAYER_1);
}

TEST_F(EnvironmentTicTacToeFixture, PrintBoard)
{
  ASSERT_NO_THROW(env.PrintBoard());
}

TEST_F(EnvironmentTicTacToeFixtureRandomMovesPlayed, ResetEnvironment)
{
  ASSERT_NE(env.GetMoveHistory().size(), 0);
  env.ResetEnvironment();
  ASSERT_EQ(env.GetMoveHistory().size(), 0);
  ASSERT_EQ(env.GetWinner(), Player::PLAYER_NONE);
  ASSERT_EQ(env.GetCurrentPlayer(), Player::PLAYER_1);
  ASSERT_TRUE(torch::equal(env.GetBoard(), torch::zeros({3, 3})));
}

TEST_F(EnvironmentTicTacToeFixture, PlayerToString)
{
  ASSERT_EQ(env.PlayerToString(Player::PLAYER_NONE), " ");
  ASSERT_EQ(env.PlayerToString(Player::PLAYER_1), "X");
  ASSERT_EQ(env.PlayerToString(Player::PLAYER_2), "O");
}

TEST_F(EnvironmentTicTacToeFixture, GetValidMoves_EmptyBoard)
{
  ASSERT_EQ(env.GetValidMoves().size(), 9);
}

TEST_F(EnvironmentTicTacToeFixture, GetValidMoves_OneMovePlayed)
{
  env.MakeMove(MoveTicTacToe(0, 0));
  ASSERT_EQ(env.GetValidMoves().size(), 8);
}

TEST_F(EnvironmentTicTacToeFixtureBoardFull, GetValidMoves_FullBoard)
{
  ASSERT_EQ(env.GetValidMoves().size(), 0);
}
