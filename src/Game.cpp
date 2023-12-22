#include "Game.hpp"

#include <utility>

#include "lib/DataManager/DataManager.hpp"
#include "lib/Logging/Logger.hpp"
#include "lib/Utilities/RandomGenerator.hpp"
#include "lib/Utilities/Time.hpp"

Game::Game(std::shared_ptr<Environment> environment, std::vector<std::shared_ptr<Agent>> const & agents, GameOptions gameOptions)
  : m_environment(std::move(environment))
  , m_agents(agents)
  , m_gameOptions(std::move(gameOptions))
{
  // reset random seed every game
  RandomGenerator::ResetSeed();
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
  auto currentPlayer = m_environment->GetCurrentPlayer();

  auto    rootNode = std::make_shared<Node>(m_environment);
  auto    mcts     = std::make_shared<MCTS>(rootNode, m_gameOptions.dirichletNoiseOptions);
  Agent * currentAgent;
  try
  {
    currentAgent = m_agents[(int)currentPlayer - 1].get();
  }
  catch (std::exception const & e)
  {
    LWARN << "Could not get agent for player " << m_environment->PlayerToString(currentPlayer);
    LWARN << "Exception: " << e.what();
    throw std::runtime_error("Could not get agent for player " + m_environment->PlayerToString(currentPlayer));
  }

  currentAgent->RunSimulations(mcts, m_gameOptions.simsPerMove);
  AddElementToMemory(rootNode->GetEnvironment(), currentPlayer, mcts->GetRoot()->GetChildren());

  // print possible moves
  LDEBUG << "Possible moves:";
  for (auto const & child: mcts->GetRoot()->GetChildren())
  {
    LDEBUG << "[" << child->GetMove()->ToString() << "] P = " << child->GetPriorProbability() << ", Q = " << child->GetQValue()
           << ", U = " << child->GetUValue() << ", N = " << child->GetVisitCount();
  }

  // based on the simulations, get the best move
  auto const & bestMove = mcts->GetBestMove(m_gameOptions.stochasticSearch);
  LINFO << "Best move: " << bestMove->ToString();
  m_environment->MakeMove(*bestMove);
}

void Game::AddElementToMemory(std::shared_ptr<Environment> const & environment, Player currentPlayer, std::vector<std::shared_ptr<Node>> const & children)
{
  // add moves to list of moves
  std::vector<std::pair<std::shared_ptr<Move>, float>> moves;
  moves.reserve(children.size());
  // get total visit count of all children
  uint totalVisitCount = 0;
  for (auto const & child: children)
  {
    totalVisitCount += child->GetVisitCount();
  }
  for (auto const & child: children)
  {
    moves.emplace_back(std::make_pair<std::shared_ptr<Move>, float>(child->GetMove(), (float)child->GetVisitCount() / (float)totalVisitCount));
  }

  // create memory element
  MemoryElement memoryElement(environment->GetBoard().clone(), currentPlayer, Player::PLAYER_NONE, moves);
  m_memory.push_back(memoryElement);
}

void Game::SaveMemoryToFile(Player winner)
{
  // set winner in all memory elements
  for (auto & element: m_memory)
  {
    element.winner = winner;
  }
  // save to file
  try
  {
    DataManager::SaveGame(m_gameOptions.memoryFolder / ("game_" + GetTimeAsString("%Y%m%d-%H%M%S") + ".bin"), m_memory);
  }
  catch (std::exception const & e)
  {
    LWARN << "Failed to save game to file. Exception: " << e.what();
    throw std::runtime_error("Failed to save game to file");
  }
}
