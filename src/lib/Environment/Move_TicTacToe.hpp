#pragma once

#include "Move.hpp"

class MoveTicTacToe : public Move
{
private:
  uint  m_row;
  uint  m_column;
  float m_priorProbability;

public:
  MoveTicTacToe(uint row, uint column, float priorProbability = 0.0F);
  ~MoveTicTacToe() override = default;

  std::pair<uint, uint> GetCoordinates() const override;
  uint                  GetRow() const override;
  uint                  GetColumn() const override;
  float                 GetPriorProbability() const override;
  void                  SetPriorProbability(float priorProbability) override;
  std::string           ToString() const override;
  size_t                GetIndex() const override;
};
