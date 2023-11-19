#pragma once

#include <sys/types.h>
#include <torch/torch.h>

#include <vector>

#include "Move.hpp"

enum class Player : uint
{
  PLAYER_NONE = 0, // used for empty spaces
  PLAYER_1,
  PLAYER_2
};

class Environment
{
public:
  virtual ~Environment() = default;

  virtual Player GetCurrentPlayer() const        = 0;
  virtual void   SetCurrentPlayer(Player player) = 0;
  virtual void   TogglePlayer()                  = 0;

  virtual void MakeMove(Move move)                      = 0;
  virtual void UndoMove()                               = 0;
  virtual bool IsValidMove(uint row, uint column) const = 0;

  [[nodiscard]] virtual std::vector<std::shared_ptr<Move>>         GetValidMoves() const  = 0;
  [[nodiscard]] virtual std::vector<std::shared_ptr<Move>> const & GetMoveHistory() const = 0;

  virtual int GetRows() const    = 0;
  virtual int GetColumns() const = 0;

  virtual Player GetPlayerAtCoordinates(uint row, uint column) const = 0;

  virtual torch::Tensor const & GetBoard()                            = 0;
  virtual void                  SetBoard(torch::Tensor const & board) = 0;

  [[nodiscard]] virtual torch::Tensor BoardToInput() const = 0;

  virtual bool   IsTerminal() const = 0;
  virtual Player GetWinner() const  = 0;

  virtual void PrintBoard() const = 0;

  virtual void ResetEnvironment() = 0;
};