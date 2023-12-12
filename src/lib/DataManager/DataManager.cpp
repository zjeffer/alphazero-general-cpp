#include "DataManager.hpp"

#include <fstream>

#include "Helpers.hpp"

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
  auto const & board = memoryElements[0].m_board;
  WriteAmount<int64_t>(outFile, board.sizes()[0]);
  WriteAmount<int64_t>(outFile, board.sizes()[1]);

  // Write the memory elements
  for (auto const & element: memoryElements)
  {
    // Write the board: torch tensor to binary
    WriteBoard(outFile, element.m_board);

    // Write the current player
    WritePlayer(outFile, element.m_currentPlayer);

    // Write the result
    WritePlayer(outFile, element.m_winner);

    // write the move history
    WriteAmount<uint>(outFile, element.m_moves.size());
    for (auto const & move: element.m_moves)
    {
      // Write the move
      WriteMove(outFile, *move.first, move.second);
    }
  }
  outFile.close();
}

template<typename MoveType>
  requires std::is_same_v<MoveType, Move>
std::vector<MemoryElement> DataManager::LoadGame(std::filesystem::path const & file)
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
  auto numElements = ReadAmount<int64_t>(inFile);
  if (numElements == 0)
  {
    throw std::runtime_error("No memory elements to load");
  }

  // Read the number of rows and columns
  auto numRows = ReadAmount<int64_t>(inFile);
  auto numCols = ReadAmount<int64_t>(inFile);
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
    Player currentPlayer = ReadPlayer(inFile, true);

    // Read the result
    Player winner = ReadPlayer(inFile, false);

    // Read the move history
    auto numMoves = ReadAmount<uint>(inFile);

    // Create the memory element
    std::vector<std::pair<std::shared_ptr<Move>, uint>> moves;
    for (int j = 0; j < numMoves; j++)
    {
      // Read the move
      auto movePair = ReadMove<MoveType>(inFile);
      auto move     = std::make_shared<Move>(movePair.first);
      for (int k = 0; k < movePair.second; k++)
      {
        move->Flip();
      }

      // Add the move to the move history
      moves.emplace_back(move);
    }
    memoryElements.emplace_back(board, static_cast<Player>(currentPlayer), static_cast<Player>(winner), moves);
  }

  return memoryElements;
}
