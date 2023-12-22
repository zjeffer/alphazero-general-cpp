#pragma once

#include <filesystem>
#include <fstream>

#include "Helpers.hpp"
#include "MemoryElement.hpp"

class DataManager
{
public:
  DataManager();
  ~DataManager() = default;

  static void SaveGame(std::filesystem::path const & file, std::vector<MemoryElement> const & memoryElements);

  template<typename MoveType>
    requires std::is_base_of_v<Move, MoveType>
  static std::vector<MemoryElement> LoadGame(std::filesystem::path const & file)
  {

    /*
     * Format:
     *  - number of memory elements
     *  - number of rows
     *  - number of columns
     *  - memory elements
     */

    std::ifstream inFile(file, std::ios::binary);
    if (!inFile)
    {
      throw std::runtime_error("Failed to open file for reading");
    }

    // Read the number of memory elements
    auto numElements = 0;
    try
    {
      numElements = ReadAmount<int64_t>(inFile);
    }
    catch (std::exception const & e)
    {
      throw std::runtime_error("Failed to read number of memory elements: " + std::string(e.what()));
    }
    if (numElements == 0)
    {
      throw std::runtime_error("No memory elements to load");
    }

    // Read the number of rows and columns
    int64_t numRows = 0;
    int64_t numCols = 0;
    try
    {
      numRows = ReadAmount<int64_t>(inFile);
      numCols = ReadAmount<int64_t>(inFile);
    }
    catch (std::exception const & e)
    {
      throw std::runtime_error("Failed to read number of rows or columns");
    }
    if (numRows == 0 || numCols == 0)
    {
      throw std::runtime_error("Invalid number of rows or columns in saved game");
    }

    // Read the memory elements
    std::vector<MemoryElement> memoryElements;
    for (int i = 0; i < numElements; i++)
    {
      // Read the board: binary to torch tensor
      torch::Tensor board = ReadBoard(inFile, numRows, numCols);

      // Read the current player
      Player currentPlayer = ReadPlayer(inFile);
      if (currentPlayer == Player::PLAYER_NONE)
      {
        throw std::runtime_error("Invalid current player");
      }

      // Read the result
      Player winner = ReadPlayer(inFile);

      // Read the move history
      uint numMoves = 0;
      try
      {
        numMoves = ReadAmount<uint>(inFile);
      }
      catch (std::exception const & e)
      {
        throw std::runtime_error("Failed to read number of moves");
      }

      // Create the memory element
      std::vector<std::pair<std::shared_ptr<Move>, float>> moves;
      for (int j = 0; j < numMoves; j++)
      {
        // Read the move and add it to the move history
        auto movePair = ReadMove<MoveType>(inFile);
        moves.emplace_back(std::make_shared<MoveType>(movePair.first), movePair.second);
      }
      memoryElements.emplace_back(board, static_cast<Player>(currentPlayer), static_cast<Player>(winner), moves);
    }

    return memoryElements;
  }
};
