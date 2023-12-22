#include "DataManager.hpp"

#include <fstream>

DataManager::DataManager() = default;

void DataManager::SaveGame(std::filesystem::path const & file, std::vector<MemoryElement> const & memoryElements)
{
  /*
   * Format:
   *  - number of memory elements
   *  - number of rows
   *  - number of columns
   *  - memory elements
   */

  if (memoryElements.empty())
  {
    throw std::runtime_error("No memory elements to save");
  }

  // ensure folder exists to save file in
  try
  {
    std::filesystem::create_directories(file.parent_path());
  }
  catch (std::exception const & e)
  {
    throw std::runtime_error("Failed to create directory for file");
  }

  std::ofstream outFile(file, std::ios::binary);
  if (!outFile)
  {
    throw std::runtime_error("Failed to open file for writing");
  }

  // Write the number of memory elements
  WriteAmount<int64_t>(outFile, memoryElements.size());

  // Write the number of rows and columns
  auto const & board = memoryElements[0].board;
  WriteAmount<int64_t>(outFile, board.sizes()[0]);
  WriteAmount<int64_t>(outFile, board.sizes()[1]);

  // Write the memory elements
  for (auto const & element: memoryElements)
  {
    // Write the board: torch tensor to binary
    WriteBoard(outFile, element.board);

    // Write the current player
    WritePlayer(outFile, element.currentPlayer);

    // Write the result
    WritePlayer(outFile, element.winner);

    // write the move history
    WriteAmount<uint>(outFile, element.moves.size());
    for (auto const & move: element.moves)
    {
      // Write the move
      WriteMove(outFile, *move.first, move.second);
    }
  }
  outFile.close();
}
