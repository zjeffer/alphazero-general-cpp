#include "Environment_TicTacToe.hpp"

#include <iostream>

#include "../Logging/Logger.hpp"

EnvironmentTicTacToe::EnvironmentTicTacToe()
  : m_board(torch::zeros({3, 3}))
{
}

Player EnvironmentTicTacToe::GetCurrentPlayer() const
{
  return m_currentPlayer;
}

void EnvironmentTicTacToe::SetCurrentPlayer(Player player)
{
  m_currentPlayer = player;
}

void EnvironmentTicTacToe::TogglePlayer()
{
  if (m_currentPlayer == Player::PLAYER_NONE)
    return;
  m_currentPlayer = (m_currentPlayer == Player::PLAYER_1) ? Player::PLAYER_2 : Player::PLAYER_1;
}

void EnvironmentTicTacToe::MakeMove(Move const & move)
{
  if (!IsValidMove(move.GetRow(), move.GetColumn()))
  {
    throw std::runtime_error("Invalid move: " + move.ToString());
  }
  m_board[move.GetRow()][move.GetColumn()] = static_cast<int>(m_currentPlayer);
  m_moveHistory.emplace_back(std::move(std::make_shared<MoveTicTacToe>(move.GetRow(), move.GetColumn(), 0.0F)));
  TogglePlayer();
}

void EnvironmentTicTacToe::UndoMove()
{
  if (m_moveHistory.empty())
  {
    throw std::runtime_error("Cannot undo move, move history is empty.");
  }
  auto * move                                = m_moveHistory.back().get();
  m_board[move->GetRow()][move->GetColumn()] = static_cast<int>(Player::PLAYER_NONE);
  m_moveHistory.pop_back();
  TogglePlayer();
}

bool EnvironmentTicTacToe::IsValidMove(uint row, uint column) const
{
  return static_cast<Player>(m_board[row][column].item<int>()) == Player::PLAYER_NONE;
}

std::vector<std::shared_ptr<Move>> EnvironmentTicTacToe::GetValidMoves() const
{
  std::vector<std::shared_ptr<Move>> validMoves;
  for (uint row = 0; row < m_board.size(0); ++row)
  {
    for (uint column = 0; column < m_board.size(1); ++column)
    {
      if (IsValidMove(row, column))
      {
        // TODO: which prior probability should be used at construction?
        validMoves.emplace_back(std::make_shared<MoveTicTacToe>(row, column, 0.0F));
      }
    }
  }
  return validMoves;
}

std::vector<std::shared_ptr<Move>> const & EnvironmentTicTacToe::GetMoveHistory() const
{
  return m_moveHistory;
}

int EnvironmentTicTacToe::GetRows() const
{
  return m_board.size(0);
}

int EnvironmentTicTacToe::GetColumns() const
{
  return m_board.size(1);
}

Player EnvironmentTicTacToe::GetPlayerAtCoordinates(uint row, uint column) const
{
  return static_cast<Player>(m_board[row][column].item<int>());
}

torch::Tensor const & EnvironmentTicTacToe::GetBoard()
{
  return m_board;
}

void EnvironmentTicTacToe::SetBoard(torch::Tensor const & board)
{
  m_board = board;
}

bool EnvironmentTicTacToe::IsTerminal() const
{
  // the game is terminal if:
  // - at least one player has three in a row
  // - the board is full and no player has three in a row

  if (GetWinner() != Player::PLAYER_NONE)
  {
    return true;
  }
  // return true if any cell contains PLAYER_NONE
  auto player = static_cast<int>(Player::PLAYER_NONE);
  return torch::any(m_board == player).item<bool>();
}

Player EnvironmentTicTacToe::GetWinner() const
{
  // the winner is the player with three in a row
  // if there is no winner (yet), return PLAYER_NONE

  // check rows
  for (uint row = 0; row < m_board.size(0); ++row)
  {
    auto cellsInRow = m_board[row];
    if (cellsInRow[0].equal(cellsInRow[1]) && cellsInRow[1].equal(cellsInRow[2]))
    {
      return static_cast<Player>(cellsInRow[0].item<int>());
    }
  }
  // check columns
  for (uint column = 0; column < m_board.size(1); ++column)
  {
    auto cellsInColumn = m_board[0][column];
    if (cellsInColumn[0].equal(cellsInColumn[1]) && cellsInColumn[1].equal(cellsInColumn[2]))
    {
      return static_cast<Player>(cellsInColumn[0].item<int>());
    }
  }
  // check diagonals
  auto cellToCheck = m_board[1][1]; // middle cell
  if (static_cast<Player>(cellToCheck.item<int>()) != Player::PLAYER_NONE)
  {
    if (m_board[0][0].equal(cellToCheck) && cellToCheck.equal(m_board[2][2]))
    {
      return static_cast<Player>(cellToCheck.item<int>());
    }
    if (m_board[0][2].equal(cellToCheck) && cellToCheck.equal(m_board[2][0]))
    {
      return static_cast<Player>(cellToCheck.item<int>());
    }
  }
  return Player::PLAYER_NONE;
}

torch::Tensor EnvironmentTicTacToe::BoardToInput() const
{
  torch::Tensor input = torch::zeros({m_board.size(0), m_board.size(1), m_board.size(1)});
  // first plane is where player 1 has pieces
  // second plane is where player 2 has pieces
  // third plane shows which player's turn it is
  input[0] = m_board.where(m_board == static_cast<int>(Player::PLAYER_1), 0);
  input[1] = m_board.where(m_board == static_cast<int>(Player::PLAYER_2), 0);
  input[2] = static_cast<int>(m_currentPlayer);
  return input.unsqueeze(0);
}

void EnvironmentTicTacToe::PrintBoard() const
{
  std::ostringstream oss;
  oss << std::endl;
  auto printDashes = [this, &oss]()
  {
    for (uint column = 0; column < m_board.size(1); ++column)
    {
      oss << "----";
    }
    oss << "-" << std::endl;
  };

  for (uint row = 0; row < m_board.size(0); ++row)
  {
    printDashes();
    for (uint column = 0; column < m_board.size(1); ++column)
    {
      oss << "| " << Player(m_board[row][column].item<int>()) << " ";
    }
    oss << "|" << std::endl;
  }
  printDashes();
  LINFO << oss.str();
}

void EnvironmentTicTacToe::ResetEnvironment()
{
  m_board = torch::zeros({m_board.size(0), m_board.size(1)});
}

std::ostream & operator<<(std::ostream & os, Player const & player)
{
  switch (player)
  {
  case Player::PLAYER_NONE:
    os << " ";
    break;
  case Player::PLAYER_1:
    os << "X";
    break;
  case Player::PLAYER_2:
    os << "O";
    break;
  }
  return os;
}

std::istream & operator>>(std::istream & is, Player & player)
{
  std::string input;
  is >> input;
  if (input == " ")
  {
    player = Player::PLAYER_NONE;
  }
  else if (input == "X")
  {
    player = Player::PLAYER_1;
  }
  else if (input == "O")
  {
    player = Player::PLAYER_2;
  }
  else
  {
    throw std::runtime_error("Invalid input for Player enum.");
  }
  return is;
}
