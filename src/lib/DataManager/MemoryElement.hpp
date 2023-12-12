#pragma once

#include <torch/types.h>

#include <cstdint>
#include <vector>

#include "../Environment/Environment.hpp"
#include "../Environment/Move.hpp"

struct MemoryElement
{
  MemoryElement(torch::Tensor board, Player currentPlayer, Player winner, std::vector<std::pair<std::shared_ptr<Move>, uint>> moves)
    : m_board(std::move(board))
    , m_currentPlayer(currentPlayer)
    , m_winner(winner)
    , m_moves(std::move(moves))
  {
  }

  torch::Tensor                                       m_board;         // board at this state
  Player                                              m_currentPlayer; // player whose turn it is
  Player                                              m_winner;        // winner of the game
  std::vector<std::pair<std::shared_ptr<Move>, uint>> m_moves;         // possible moves at this state with their visit counts
};