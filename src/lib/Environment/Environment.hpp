#pragma once

#include <sys/types.h>
#include <torch/torch.h>

#include <vector>

enum class Player : uint;
class Move;

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

  [[nodiscard]] virtual std::vector<Move> GetValidMoves() const  = 0;
  [[nodiscard]] virtual std::vector<Move> GetMoveHistory() const = 0;

  virtual int GetRows() const    = 0;
  virtual int GetColumns() const = 0;

  virtual Player GetPlayerAtCoordinates(uint row, uint column) const = 0;

  virtual torch::Tensor const & GetBoard()                            = 0;
  virtual void                  SetBoard(torch::Tensor const & board) = 0;

  virtual bool   IsTerminal() const = 0;
  virtual Player GetWinner() const  = 0;

  virtual void PrintBoard() const = 0;

  virtual void ResetEnvironment() = 0;
};