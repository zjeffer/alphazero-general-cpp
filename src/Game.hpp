#pragma once

#include <memory>

#include "lib/Agent/Agent.hpp"
#include "lib/DataManager/MemoryElement.hpp"
#include "lib/Environment/Environment.hpp"

struct GameOptions
{
  bool saveMemory;                             // if true, save the memory to a file after each game
  uint maxMoves;                               // the maximum amount of moves in a game
  uint simsPerMove;                            // the amount of MCTS simulations per move
  bool stochasticSearch              = true;   // if true, don't play the best move but use a stochastic distribution to select a move based on the visit counts
  std::filesystem::path memoryFolder = "data"; // the folder to save the games to. Each game will be saved in its own file
};

class Game
{
private:
  std::shared_ptr<Environment> m_environment;
  std::unique_ptr<Agent>       m_agent1;
  std::unique_ptr<Agent>       m_agent2;
  GameOptions                  m_gameOptions;
  uint                         m_gameID;

  std::vector<MemoryElement> m_memory;

public:
  Game(std::shared_ptr<Environment> environment, std::unique_ptr<Agent> agent1, std::unique_ptr<Agent> agent2, GameOptions gameOptions);
  ~Game() = default;

  void PlayGame();
  void PlayMove();

private:
  void AddElementToMemory(std::shared_ptr<Environment> const & environment, Player currentPlayer, std::vector<std::shared_ptr<Node>> const & children);
  void SaveMemoryToFile(Player winner);
};