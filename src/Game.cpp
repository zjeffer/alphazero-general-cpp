#include "Game.hpp"

Game::Game(std::unique_ptr<Environment> environment, std::unique_ptr<Agent> agent1, std::unique_ptr<Agent> agent2, GameOptions gameOptions)
  : m_environment(std::move(environment))
  , m_agent1(std::move(agent1))
  , m_agent2(std::move(agent2))
  , m_gameOptions(gameOptions)
{
}

void Game::PlayGame() {

}

void Game::PlayMove() {}

void Game::AddElementToMemory() {}

void Game::SaveMemoryToFile() {}
