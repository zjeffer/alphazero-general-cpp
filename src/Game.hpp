#pragma once

#include <memory>

#include "lib/Agent/Agent.hpp"
#include "lib/DataManager/MemoryElement.hpp"
#include "lib/Environment/Environment.hpp"

struct GameOptions
{
  bool saveMemory = true;                      // if true, save the memory to a file after each game
  uint maxMoves;                               // the maximum amount of moves in a game
  uint simsPerMove;                            // the amount of MCTS simulations per move
  bool stochasticSearch              = true;   // if true, don't play the best move but use a stochastic distribution to select a move based on the visit counts
  std::filesystem::path memoryFolder = "data"; // the folder to save the games to. Each game will be saved in its own file
  bool                  useDirichletNoise = true; // if true, add dirichlet noise to the root node on every move
  DirichletNoiseOptions dirichletNoiseOptions;    // alpha and beta for the dirichlet noise which is added to the root node on every move
};

class Game
{
private:
  std::shared_ptr<Environment>        m_environment;
  std::vector<std::shared_ptr<Agent>> m_agents;
  GameOptions                         m_gameOptions;
  uint                                m_gameID;

  std::vector<MemoryElement> m_memory;

public:
  Game(std::shared_ptr<Environment> environment, std::vector<std::shared_ptr<Agent>> const & agents, GameOptions gameOptions);
  ~Game() = default;

  void PlayGame();
  void PlayMove();

private:
  void AddElementToMemory(std::shared_ptr<Environment> const & environment, Player currentPlayer, std::vector<std::shared_ptr<Node>> const & children);
  void SaveMemoryToFile(Player winner);
};