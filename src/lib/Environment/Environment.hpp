#pragma once

#include <sys/types.h>
#include <torch/torch.h>

#include <vector>

#include "Move.hpp"

enum class Player : uint8_t
{
  PLAYER_NONE = 0, // used for empty spaces
  PLAYER_1,
  PLAYER_2
};

// stream operators for Player
std::ostream & operator<<(std::ostream & os, Player const & player);
std::istream & operator>>(std::istream & is, Player & player);

class Environment
{
public:
  virtual ~Environment() = default;

  [[nodiscard]] virtual std::unique_ptr<Environment> Clone() const = 0;

  virtual Player GetCurrentPlayer() const        = 0;
  virtual void   SetCurrentPlayer(Player player) = 0;
  virtual void   TogglePlayer()                  = 0;

  virtual void MakeMove(Move const & move)              = 0;
  virtual void UndoMove()                               = 0;
  virtual bool IsValidMove(uint row, uint column) const = 0;

  [[nodiscard]] virtual std::vector<std::shared_ptr<Move>>         GetValidMoves() const  = 0;
  [[nodiscard]] virtual std::vector<std::shared_ptr<Move>> const & GetMoveHistory() const = 0;

  virtual int GetRows() const    = 0;
  virtual int GetColumns() const = 0;

  virtual Player GetPlayerAtCoordinates(uint row, uint column) const = 0;

  virtual torch::Tensor const & GetBoard() const                                            = 0;
  virtual void                  SetBoard(torch::Tensor const & board, Player currentPlayer) = 0;

  [[nodiscard]] virtual torch::Tensor BoardToInput() const = 0;

  virtual bool   IsTerminal() const = 0;
  virtual Player GetWinner() const  = 0;

  virtual void PrintBoard() const = 0;

  virtual void ResetEnvironment() = 0;

  virtual std::string PlayerToString(Player player) const = 0;
};