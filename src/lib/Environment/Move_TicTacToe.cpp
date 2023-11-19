#include "Move_TicTacToe.hpp"

MoveTicTacToe::MoveTicTacToe(uint row, uint column, float priorProbability)
  : m_row(row)
  , m_column(column)
  , m_priorProbability(priorProbability)
{
}

std::pair<uint, uint> MoveTicTacToe::GetCoordinates() const
{
  return std::make_pair(m_row, m_column);
}

uint MoveTicTacToe::GetRow() const
{
  return m_row;
}

uint MoveTicTacToe::GetColumn() const
{
  return m_column;
}

float MoveTicTacToe::GetPriorProbability() const
{
  return m_priorProbability;
}

void MoveTicTacToe::SetPriorProbability(float priorProbability)
{
  m_priorProbability = priorProbability;
}
