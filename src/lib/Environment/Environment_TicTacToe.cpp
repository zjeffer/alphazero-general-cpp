#include "Environment_TicTacToe.hpp"

#include <iostream>

#include "../Logging/Logger.hpp"

namespace
{
auto constexpr BOARD_SIZE_ROWS = 3;
auto constexpr BOARD_SIZE_COLS = 3;
auto constexpr INPUT_PLANES    = 3; // 3 planes of rows * columns: player 1, player 2, current player
} // namespace

EnvironmentTicTacToe::EnvironmentTicTacToe()
  : m_board(torch::zeros({BOARD_SIZE_ROWS, BOARD_SIZE_COLS}))
{
}

EnvironmentTicTacToe::EnvironmentTicTacToe(EnvironmentTicTacToe const & other)
  : m_board(other.GetBoard().clone()) // clone the board because otherwise it will be a reference to the original board
  , m_moveHistory(other.GetMoveHistory())
  , m_currentPlayer(other.GetCurrentPlayer())
{
}

std::unique_ptr<Environment> EnvironmentTicTacToe::Clone() const
{
  return std::make_unique<EnvironmentTicTacToe>(EnvironmentTicTacToe(*this));
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

torch::Tensor const & EnvironmentTicTacToe::GetBoard() const
{
  return m_board;
}

void EnvironmentTicTacToe::SetBoard(torch::Tensor const & board, Player currentPlayer)
{
  m_board = board;
  m_moveHistory.clear();
  SetCurrentPlayer(currentPlayer);
}

bool EnvironmentTicTacToe::IsTerminal() const
{
  // the game is terminal if:
  // - at least one player has three in a row
  // - or the board is full

  if (GetWinner() != Player::PLAYER_NONE)
  {
    return true;
  }
  return BoardIsFull();
}

Player EnvironmentTicTacToe::GetWinner() const
{
  // the winner is the player with three in a row
  // if there is no winner (yet), return PLAYER_NONE

  // check horizontal win
  for (uint row = 0; row < m_board.size(0); ++row)
  {
    auto cellsInRow = m_board[row];
    if (cellsInRow[0].equal(cellsInRow[1]) && cellsInRow[1].equal(cellsInRow[2]))
    {
      return static_cast<Player>(cellsInRow[0].item<int>());
    }
  }
  // check vertical win
  for (uint column = 0; column < m_board.size(1); ++column)
  {
    auto cellsInColumn = m_board.transpose(0, 1)[column];
    if (cellsInColumn[0].equal(cellsInColumn[1]) && cellsInColumn[1].equal(cellsInColumn[2]))
    {
      return static_cast<Player>(cellsInColumn[0].item<int>());
    }
  }
  // check diagonal win
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
  torch::Tensor input = torch::zeros({INPUT_PLANES, m_board.size(0), m_board.size(1)});
  // first plane is where player 1 has pieces
  // second plane is where player 2 has pieces
  // third plane shows which player's turn it is
  input[0] = m_board.where(m_board == static_cast<int>(Player::PLAYER_1), 0);
  input[1] = m_board.where(m_board == static_cast<int>(Player::PLAYER_2), 0);
  input[2] = torch::full({m_board.size(0), m_board.size(1)}, static_cast<int>(m_currentPlayer));
  return input.unsqueeze(0);
}

void EnvironmentTicTacToe::PrintBoard() const
{
  LINFO << "Current player: " << PlayerToString(m_currentPlayer);
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
      oss << "| " << PlayerToString(static_cast<Player>(m_board[row][column].item<int>())) << " ";
    }
    oss << "|" << std::endl;
  }
  printDashes();
  LINFO << oss.str();
}

void EnvironmentTicTacToe::ResetEnvironment()
{
  m_board = torch::zeros({m_board.size(0), m_board.size(1)});
  m_moveHistory.clear();
}

std::string EnvironmentTicTacToe::PlayerToString(Player player) const
{
  switch (player)
  {
  case Player::PLAYER_NONE:
    return " ";
  case Player::PLAYER_1:
    return "X";
  case Player::PLAYER_2:
    return "O";
  }
  throw std::runtime_error("Invalid player");
}

bool EnvironmentTicTacToe::BoardIsFull() const
{
  auto none = static_cast<int>(Player::PLAYER_NONE);
  return torch::all(m_board != none).item<bool>();
}
