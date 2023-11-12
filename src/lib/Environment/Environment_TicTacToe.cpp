#include "Environment_TicTacToe.hpp"

#include <iostream>

Move::Move(uint row, uint column)
  : m_row(row)
  , m_column(column)
{
}

std::pair<uint, uint> Move::GetCoordinates() const
{
  return std::make_pair(m_row, m_column);
}

uint Move::GetRow() const
{
  return m_row;
}

uint Move::GetColumn() const
{
  return m_column;
}

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
  m_currentPlayer = (m_currentPlayer == Player::PLAYER_X) ? Player::PLAYER_O : Player::PLAYER_X;
}

void EnvironmentTicTacToe::MakeMove(Move move)
{
  if (!IsValidMove(move.GetRow(), move.GetColumn()))
  {
    throw std::runtime_error("Invalid move.");
  }
  m_board[move.GetRow()][move.GetColumn()] = static_cast<int>(m_currentPlayer);
  m_moveHistory.emplace_back(move);
  TogglePlayer();
}

void EnvironmentTicTacToe::UndoMove()
{
  if (m_moveHistory.empty())
  {
    throw std::runtime_error("Cannot undo move, move history is empty.");
  }
  auto move                                = m_moveHistory.back();
  m_board[move.GetRow()][move.GetColumn()] = static_cast<int>(Player::PLAYER_NONE);
  m_moveHistory.pop_back();
  TogglePlayer();
}

bool EnvironmentTicTacToe::IsValidMove(uint row, uint column) const
{
  return static_cast<Player>(m_board[row][column].item<int>()) == Player::PLAYER_NONE;
}

std::vector<Move> EnvironmentTicTacToe::GetValidMoves() const
{
  std::vector<Move> validMoves;
  for (uint row = 0; row < m_board.size(0); ++row)
  {
    for (uint column = 0; column < m_board.size(1); ++column)
    {
      if (IsValidMove(row, column))
      {
        validMoves.emplace_back(row, column);
      }
    }
  }
  return validMoves;
}

std::vector<Move> EnvironmentTicTacToe::GetMoveHistory() const
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

void EnvironmentTicTacToe::PrintBoard() const
{
  for (uint row = 0; row < m_board.size(0); ++row)
  {
    std::cout << "| ";
    for (uint column = 0; column < m_board.size(1); ++column)
    {
      std::cout << Player(m_board[row][column].item<int>());
      if (column < m_board.size(1) - 1)
      {
        std::cout << " | ";
      }
    }
    std::cout << std::endl;
  }
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
  case Player::PLAYER_X:
    os << "X";
    break;
  case Player::PLAYER_O:
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
    player = Player::PLAYER_X;
  }
  else if (input == "O")
  {
    player = Player::PLAYER_O;
  }
  else
  {
    throw std::runtime_error("Invalid input for Player enum.");
  }
  return is;
}
