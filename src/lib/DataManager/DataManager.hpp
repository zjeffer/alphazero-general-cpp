#pragma once

#include <filesystem>

#include "MemoryElement.hpp"

class DataManager
{
public:
  DataManager();
  ~DataManager() = default;

  static void SaveGame(std::filesystem::path const & file, std::vector<MemoryElement> const & memoryElements);

  template<typename MoveType>
    requires std::is_same_v<MoveType, Move>
  static std::vector<MemoryElement> LoadGame(std::filesystem::path const & file);
};