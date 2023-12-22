#pragma once

#include <fstream>

#include "../Environment/Environment.hpp"

inline torch::Tensor ReadBoard(std::ifstream & inFile, int64_t rows, int64_t cols)
{
  std::vector<int64_t> boardData(rows * cols);
  for (auto i = 0; i < rows * cols; i++)
  {
    try
    {
      inFile.read(reinterpret_cast<char *>(&boardData[i]), sizeof(boardData[i]));
    }
    catch (std::exception const & e)
    {
      throw std::runtime_error("Failed to read board at index " + std::to_string(i) + ". Exception: " + e.what());
    }
    if (inFile.fail())
    {
      throw std::runtime_error("Failed to read board");
    }
    if (boardData[i] < 0 || boardData[i] > 2)
    {
      throw std::runtime_error("Invalid board value at index " + std::to_string(i));
    }
  }
  return torch::tensor(boardData).reshape({rows, cols}).to(torch::kInt64);
}

inline void WriteBoard(std::ofstream & outFile, torch::Tensor const & board)
{
  // reshape the board to a vector
  auto   boardData    = board.reshape({board.numel()}).to(torch::kInt64).to(torch::kCPU);
  auto * boardDataPtr = boardData.data_ptr<int64_t>();
  // write the board
  for (auto i = 0; i < boardData.numel(); i++)
  {
    try
    {
      outFile.write(reinterpret_cast<char const *>(&boardDataPtr[i]), sizeof(boardDataPtr[i]));
    }
    catch (std::exception const & e)
    {
      throw std::runtime_error("Failed to write board at index " + std::to_string(i) + ". Exception: " + e.what());
    }
    if (outFile.fail())
    {
      throw std::runtime_error("Failed to write board");
    }
  }
}

inline Player ReadPlayer(std::ifstream & inFile)
{
  Player player;
  inFile.read(reinterpret_cast<char *>(&player), sizeof(Player));
  if (inFile.fail())
  {
    throw std::runtime_error("Failed to read player");
  }
  return player;
}

inline void WritePlayer(std::ofstream & outFile, Player player)
{
  outFile.write(reinterpret_cast<char const *>(&player), sizeof(Player));
  if (outFile.fail())
  {
    throw std::runtime_error("Failed to write player");
  }
}

template<typename MoveType>
  requires std::is_base_of_v<Move, MoveType>
std::pair<MoveType, float> ReadMove(std::ifstream & inFile)
{
  uint moveRow;
  inFile.read(reinterpret_cast<char *>(&moveRow), sizeof(moveRow));
  if (inFile.fail())
  {
    throw std::runtime_error("Failed to read move row");
  }
  uint moveCol;
  inFile.read(reinterpret_cast<char *>(&moveCol), sizeof(moveCol));
  if (inFile.fail())
  {
    throw std::runtime_error("Failed to read move column");
  }
  float probability;
  inFile.read(reinterpret_cast<char *>(&probability), sizeof(probability));
  if (inFile.fail())
  {
    throw std::runtime_error("Failed to read move probability");
  }
  if (probability < 0 || probability > 1)
  {
    throw std::runtime_error("Invalid move probability, must be in range [0, 1]");
  }
  return {MoveType{moveRow, moveCol, probability}, probability};
}

template<typename MoveType>
  requires std::is_same_v<MoveType, Move>
void WriteMove(std::ofstream & outFile, MoveType const & move, float probability)
{
  // write with padding
  auto row = move.GetRow();
  outFile.write(reinterpret_cast<char const *>(&row), sizeof(row));
  if (outFile.fail())
  {
    throw std::runtime_error("Failed to write move row");
  }
  auto col = move.GetColumn();
  outFile.write(reinterpret_cast<char const *>(&col), sizeof(col));
  if (outFile.fail())
  {
    throw std::runtime_error("Failed to write move column");
  }
  outFile.write(reinterpret_cast<char const *>(&probability), sizeof(probability));
  if (outFile.fail())
  {
    throw std::runtime_error("Failed to write move probability");
  }
}

template<typename T>
T ReadAmount(std::ifstream & inFile)
{
  T amount;
  inFile.read(reinterpret_cast<char *>(&amount), sizeof(T));
  if (inFile.fail())
  {
    throw std::runtime_error("Failed to read amount");
  }
  return amount;
}

template<typename T>
void WriteAmount(std::ofstream & outFile, T amount)
{
  outFile.write(reinterpret_cast<char const *>(&amount), sizeof(T));
  if (outFile.fail())
  {
    throw std::runtime_error("Failed to write amount");
  }
}