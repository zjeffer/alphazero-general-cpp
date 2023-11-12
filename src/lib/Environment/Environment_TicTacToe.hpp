#pragma once

#include "Environment.hpp"

enum class Player : uint
{
  PLAYER_NONE = 0, // used for empty spaces
  PLAYER_X,
  PLAYER_O
};

class Move
{
private:
  uint m_row;
  uint m_column;

public:
  Move(uint row, uint column);
  ~Move() = default;

  std::pair<uint, uint> GetCoordinates() const;
  uint                  GetRow() const;
  uint                  GetColumn() const;
};

// create iostreams for Player enum
std::ostream & operator<<(std::ostream & os, Player const & player);
std::istream & operator>>(std::istream & is, Player & player);

class EnvironmentTicTacToe : public Environment
{
private:
  torch::Tensor m_board;

  Player            m_currentPlayer = Player::PLAYER_X;
  std::vector<Move> m_moveHistory;

public:
  EnvironmentTicTacToe();
  ~EnvironmentTicTacToe() override = default;

  Player GetCurrentPlayer() const override;
  void   SetCurrentPlayer(Player player) override;
  void   TogglePlayer() override;

  void MakeMove(Move move) override;
  void UndoMove() override;
  bool IsValidMove(uint row, uint column) const override;

  [[nodiscard]] std::vector<Move> GetValidMoves() const override;
  [[nodiscard]] std::vector<Move> GetMoveHistory() const override;

  int GetRows() const override;
  int GetColumns() const override;

  Player GetPlayerAtCoordinates(uint row, uint column) const override;

  torch::Tensor const & GetBoard() override;
  void                  SetBoard(torch::Tensor const & board) override;

  bool   IsTerminal() const override;
  Player GetWinner() const override;

  void PrintBoard() const override;

  void ResetEnvironment() override;
};
