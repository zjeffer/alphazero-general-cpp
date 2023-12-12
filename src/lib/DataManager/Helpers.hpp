#pragma once

#include <fstream>

#include "../Environment/Environment.hpp"

torch::Tensor ReadBoard(std::ifstream & inFile, int64_t rows, int64_t cols)
{
  torch::Tensor board            = torch::zeros({rows, cols}, torch::kByte);
  auto *        boardDataPointer = board.data_ptr<uint8_t>();
  inFile >> *boardDataPointer;
  if (inFile.fail())
  {
    throw std::runtime_error("Failed to read board");
  }
  return board;
}

void WriteBoard(std::ofstream & outFile, torch::Tensor const & board)
{
  auto const & boardSizes = board.sizes();

  if (outFile.fail())
  {
    throw std::runtime_error("Failed to write board");
  }
}

Player ReadPlayer(std::ifstream & inFile, bool throwIfPlayerIsNone = false)
{
  Player player;
  inFile >> player;
  if (inFile.fail())
  {
    throw std::runtime_error("Failed to read player");
  }
  if (throwIfPlayerIsNone && player == Player::PLAYER_NONE)
  {
    throw std::runtime_error("Invalid player");
  }
  return player;
}

void WritePlayer(std::ofstream & outFile, Player player)
{
  outFile << player;
  if (outFile.fail())
  {
    throw std::runtime_error("Failed to write player");
  }
}

template<typename MoveType>
  requires std::is_same_v<MoveType, Move>
std::pair<MoveType, uint> ReadMove(std::ifstream & inFile)
{
  uint moveRow;
  uint moveCol;
  inFile >> moveRow;
  if (inFile.fail())
  {
    throw std::runtime_error("Failed to read move row");
  }
  inFile >> moveCol;
  if (inFile.fail())
  {
    throw std::runtime_error("Failed to read move column");
  }
  float probability;
  inFile >> probability;
  if (inFile.fail())
  {
    throw std::runtime_error("Failed to read move probability");
  }
  uint visits;
  inFile >> visits;
  if (inFile.fail())
  {
    throw std::runtime_error("Failed to read move visits");
  }
  return {MoveType{moveRow, moveCol, probability}, visits};
}

template<typename MoveType>
  requires std::is_same_v<MoveType, Move>
void WriteMove(std::ofstream & outFile, MoveType const & move, uint visits)
{
  outFile << move.GetRow();
  if (outFile.fail())
  {
    throw std::runtime_error("Failed to write move row");
  }
  outFile << move.GetColumn();
  if (outFile.fail())
  {
    throw std::runtime_error("Failed to write move column");
  }
  outFile << move.GetPriorProbability();
  if (outFile.fail())
  {
    throw std::runtime_error("Failed to write move");
  }
  outFile << visits;
  if (outFile.fail())
  {
    throw std::runtime_error("Failed to write move visits");
  }
}

template<typename T>
T ReadAmount(std::ifstream & inFile)
{
  T amount;
  inFile >> amount;
  if (inFile.fail())
  {
    throw std::runtime_error("Failed to read amount");
  }
  return amount;
}

template<typename T>
void WriteAmount(std::ofstream & outFile, T amount)
{
  outFile << amount;
  if (outFile.fail())
  {
    throw std::runtime_error("Failed to write amount");
  }
}