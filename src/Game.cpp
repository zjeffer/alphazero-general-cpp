#include "Game.hpp"

#include "lib/Logging/Logger.hpp"
#include "lib/Utilities/RandomGenerator.hpp"

Game::Game(std::shared_ptr<Environment> environment, std::unique_ptr<Agent> agent1, std::unique_ptr<Agent> agent2, GameOptions gameOptions)
  : m_environment(std::move(environment))
  , m_agent1(std::move(agent1))
  , m_agent2(std::move(agent2))
  , m_gameOptions(gameOptions)
{
  // reset random seed
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

  // game is terminal, get winner
}

void Game::PlayMove()
{
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

  // TODO: add element to memory
  // TODO: figure out if adding to memory should be done before or after making the move
  // previous implementation added to memory before making the move

  // based on the simulations, get the best move
  auto const & bestMove = mcts->GetBestMove(m_gameOptions.stochasticSearch);
  LINFO << "Best move: " << bestMove.ToString() << " with probability " << bestMove.GetPriorProbability();
  m_environment->MakeMove(bestMove);
  m_environment->PrintBoard();
}

void Game::AddElementToMemory() {}

void Game::SaveMemoryToFile() {}
