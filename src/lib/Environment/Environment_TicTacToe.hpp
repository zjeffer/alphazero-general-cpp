#pragma once

#include "Environment.hpp"
#include "Move_TicTacToe.hpp"

class EnvironmentTicTacToe : public Environment
{
private:
  torch::Tensor m_board;

  Player                             m_currentPlayer = Player::PLAYER_1;
  std::vector<std::shared_ptr<Move>> m_moveHistory;

public:
  EnvironmentTicTacToe();
  ~EnvironmentTicTacToe() override = default;

  EnvironmentTicTacToe(EnvironmentTicTacToe const & other);

  [[nodiscard]] std::unique_ptr<Environment> Clone() const override;

  Player GetCurrentPlayer() const override;
  void   SetCurrentPlayer(Player player) override;
  void   TogglePlayer() override;

  void MakeMove(Move const & move) override;
  void UndoMove() override;
  bool IsValidMove(uint row, uint column) const override;

  [[nodiscard]] std::vector<std::shared_ptr<Move>>         GetValidMoves() const override;
  [[nodiscard]] std::vector<std::shared_ptr<Move>> const & GetMoveHistory() const override;

  int GetRows() const override;
  int GetColumns() const override;

  Player GetPlayerAtCoordinates(uint row, uint column) const override;

  torch::Tensor const & GetBoard() const override;
  void                  SetBoard(torch::Tensor const & board, Player currentPlayer) override;

  [[nodiscard]] torch::Tensor BoardToInput() const override;

  bool   IsTerminal() const override;
  Player GetWinner() const override;

  void PrintBoard() const override;

  void ResetEnvironment() override;

  std::string PlayerToString(Player player) const override;

private:
  bool BoardIsFull() const;
};
