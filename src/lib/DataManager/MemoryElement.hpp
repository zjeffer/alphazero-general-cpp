#pragma once

#include <torch/types.h>

#include <cstdint>
#include <vector>

#include "../Environment/Environment.hpp"
#include "../Environment/Move.hpp"

struct MemoryElement
{
  MemoryElement(torch::Tensor board_, Player currentPlayer_, Player winner_, std::vector<std::pair<std::shared_ptr<Move>, float>> moves_)
    : board(std::move(board_))
    , currentPlayer(currentPlayer_)
    , winner(winner_)
    , moves(std::move(moves_))
  {
  }

  // override this to account for different games
  // default implementation: 3 x,y planes:
  // 1 plane for player1's pieces
  // 1 plane for player2's pieces
  // 1 plane indicating the current player
  // output:
  // x elements for the possible moves (represented by floats [0, 1])
  // 1 element for the winner
  virtual std::pair<torch::Tensor, torch::Tensor> ConvertToInputAndOutput() const
  {
    torch::Tensor input = torch::zeros({3, board.size(0), board.size(1)});
    try
    {
      input[0] = board.where(board == static_cast<int>(Player::PLAYER_1), 0);
      input[1] = board.where(board == static_cast<int>(Player::PLAYER_2), 0);
      input[2] = board.where(board == static_cast<int>(currentPlayer), 0);
      input    = input.unsqueeze(0);
    }
    catch (std::exception const & e)
    {
      throw std::runtime_error("Failed to convert board to input tensor. Exception: " + std::string(e.what()));
    }

    // policy + value head = #moves + 1
    // default implementation: amount of moves is amount of squares of the board
    torch::Tensor output = torch::zeros({static_cast<long>(board.size(0) * board.size(1) + 1)});
    try
    {
      for (auto const & [move, prob]: moves)
      {
        output[move->GetIndex()] = prob;
      }
      output[output.size(0) - 1] = static_cast<float>(winner);
      output                     = output.unsqueeze(0);
    }
    catch (std::exception const & e)
    {
      throw std::runtime_error("Failed to convert moves to output tensor. Exception: " + std::string(e.what()));
    }

    return {input, output};
  }

  torch::Tensor                                        board;         // board at this state
  Player                                               currentPlayer; // player whose turn it is
  Player                                               winner;        // winner of the game
  std::vector<std::pair<std::shared_ptr<Move>, float>> moves;         // possible moves at this state with their visit counts
};