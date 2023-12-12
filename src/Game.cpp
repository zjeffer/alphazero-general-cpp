#include "Game.hpp"

#include <utility>

#include "lib/DataManager/DataManager.hpp"
#include "lib/Logging/Logger.hpp"
#include "lib/Utilities/RandomGenerator.hpp"

Game::Game(std::shared_ptr<Environment> environment, std::unique_ptr<Agent> agent1, std::unique_ptr<Agent> agent2, GameOptions gameOptions)
  : m_environment(std::move(environment))
  , m_agent1(std::move(agent1))
  , m_agent2(std::move(agent2))
  , m_gameOptions(std::move(gameOptions))
{
  // reset random seed
  RandomGenerator::ResetSeed();

  // generate random game ID (for saving memory)
  m_gameID = RandomGenerator::GenerateRandomNumber(10000, 99999);
}

void Game::PlayGame()
{
  LINFO << "Starting new game";
  uint moveCounter = 0;
  while (!m_environment->IsTerminal())
  {
    LINFO << "Move " << moveCounter + 1;
    m_environment->PrintBoard();
    if (moveCounter > m_gameOptions.maxMoves)
    {
      LINFO << "Game exceeded max moves";
      break;
    }

    PlayMove();
    moveCounter++;
  }

  m_environment->PrintBoard();

  // game is terminal, get winner
  auto winner = m_environment->GetWinner();
  LINFO << "Winner: " << m_environment->PlayerToString(winner);
  SaveMemoryToFile(winner);
}

void Game::PlayMove()
{
  LINFO << "=================== Playing move ===================";
  // run simulations
  auto    currentPlayer = m_environment->GetCurrentPlayer();
  Agent * currentAgent;
  switch (currentPlayer)
  {
  case Player::PLAYER_1:
    currentAgent = m_agent1.get();
    break;
  case Player::PLAYER_2:
    currentAgent = m_agent2.get();
    break;
  case Player::PLAYER_NONE:
    LWARN << "Invalid player";
    exit(1);
  }

  auto rootNode = std::make_shared<Node>(m_environment);
  auto mcts     = std::make_shared<MCTS>(rootNode);
  currentAgent->RunSimulations(mcts, m_gameOptions.simsPerMove);

  AddElementToMemory(rootNode->GetEnvironment(), currentPlayer, mcts->GetRoot()->GetChildren());

  // print possible moves
  LDEBUG << "Possible moves:";
  for (auto const & child: mcts->GetRoot()->GetChildren())
  {
    LDEBUG << "[" << child->GetMove()->ToString() << "] Q = " << child->GetQValue() << ", U = " << child->GetUValue() << ", N = " << child->GetVisitCount();
  }

  // based on the simulations, get the best move
  auto const & bestMove = mcts->GetBestMove(m_gameOptions.stochasticSearch);
  LINFO << "Best move: " << bestMove->ToString() << " with probability " << bestMove->GetPriorProbability();
  m_environment->MakeMove(*bestMove);
}

void Game::AddElementToMemory(std::shared_ptr<Environment> const & environment, Player currentPlayer, std::vector<std::shared_ptr<Node>> const & children)
{
  // add moves to list of moves
  std::vector<std::pair<std::shared_ptr<Move>, uint>> moves;
  moves.reserve(children.size());
  for (auto const & child: children)
  {
    moves.emplace_back(std::make_pair<std::shared_ptr<Move>, uint>(child->GetMove(), child->GetVisitCount()));
  }

  // create memory element
  MemoryElement memoryElement(environment->GetBoard(), currentPlayer, Player::PLAYER_NONE, moves);
  m_memory.push_back(memoryElement);
}

void Game::SaveMemoryToFile(Player winner)
{
  // set winner in all memory elements
  for (auto & element: m_memory)
  {
    element.m_winner = winner;
  }
  // save to file
  DataManager::SaveGame(m_gameOptions.memoryFolder / ("game_" + std::to_string(m_gameID) + ".bin"), m_memory);
}
