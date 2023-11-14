#pragma once

#include <memory>

#include "lib/Agent/Agent.hpp"
#include "lib/Environment/Environment.hpp"

struct GameOptions
{
  bool saveMemory;
  uint maxMoves;
  uint simsPerMove;
};

class Game
{
private:
  std::unique_ptr<Environment> m_environment;
  std::unique_ptr<Agent>       m_agent1;
  std::unique_ptr<Agent>       m_agent2;
  GameOptions                  m_gameOptions;

public:
  Game(std::unique_ptr<Environment> environment, std::unique_ptr<Agent> agent1, std::unique_ptr<Agent> agent2, GameOptions gameOptions);
  ~Game() = default;

  void PlayGame();
  void PlayMove();

private:
  void AddElementToMemory();
  void SaveMemoryToFile();
};