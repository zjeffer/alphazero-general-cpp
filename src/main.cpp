#include <iostream>

#include "Game.hpp"
#include "lib/ArgumentParsing/ArgumentParser.hpp"
#include "lib/Configuration/Configuration.hpp"
#include "lib/DataManager/DataManager.hpp"
#include "lib/Environment/Environment_TicTacToe.hpp"
#include "lib/Logging/Logger.hpp"
#include "lib/NeuralNetwork/NeuralNetwork.hpp"
#include "lib/Utilities/RandomGenerator.hpp"

Logger logger;

// create static generator
std::mt19937 RandomGenerator::generator(std::random_device{}());

void CreateModel(Arguments const & arguments)
{
  // create model
  try
  {
    LINFO << "Creating model with architecture from " << arguments.networkArchitecturePath.string() << " and saving to " << arguments.modelFolder.string();
    if (std::filesystem::exists(arguments.modelFolder / "model.pt"))
    {
      throw std::runtime_error("Model already exists in " + arguments.modelFolder.string() + " folder");
    }
    if (!std::filesystem::exists(arguments.networkArchitecturePath))
    {
      throw std::runtime_error("Network architecture file does not exist: " + arguments.networkArchitecturePath.string());
    }
    auto networkArchitecture = NetworkArchitecture(arguments.networkArchitecturePath);
    auto network             = NeuralNetwork(networkArchitecture);
    network.SaveModel(arguments.modelFolder); // will save the model and architecture file
  }
  catch (std::exception const & e)
  {
    LWARN << "Failed to create model. Exception: \n" << e.what();
    throw std::runtime_error("Failed to create model. Exception: \n" + std::string(e.what()));
  }
}

void SelfPlay(Arguments const & arguments)
{
  auto agentOptions        = AgentOptions(arguments.agentConfigPath);
  auto gameOptions         = GameOptions(arguments.gameConfigPath);
  gameOptions.memoryFolder = arguments.dataFolder;

  // load or create model
  std::shared_ptr<NeuralNetwork> neuralNetwork;
  if (std::filesystem::exists(arguments.modelFolder / "model.pt"))
  {
    LINFO << "Model exists. Loading model from " << arguments.modelFolder.string() << " folder";
    if (!std::filesystem::exists(arguments.modelFolder / "model.jsonc"))
    {
      throw std::runtime_error("Model architecture file does not exist in " + arguments.modelFolder.string() + " folder");
    }
    // load existing model
    neuralNetwork = std::make_unique<NeuralNetwork>(arguments.modelFolder);
  }

  // create agents
  std::vector<std::shared_ptr<Agent>> agents;
  agents.reserve(agentOptions.agentNames.size());
  for (auto const & agentName: agentOptions.agentNames)
  {
    agents.emplace_back(std::make_unique<Agent>(agentName, neuralNetwork));
  }

  // keep tally of wins
  std::map<Player, uint> wins;
  uint                   totalGames = 0;
  while (true)
  {
    Game game   = Game(std::make_unique<EnvironmentTicTacToe>(), agents, gameOptions);
    auto winner = game.PlayGame();
    wins[winner]++;
    totalGames++;
    LINFO << "Tally after playing " << totalGames << " game(s): \n"
          << "  Player 1: " << wins[Player::PLAYER_1] << "\n"
          << "  Player 2: " << wins[Player::PLAYER_2] << "\n"
          << "  Draws:    " << wins[Player::PLAYER_NONE] << "\n";
  }
}

void Evaluate(Arguments const & arguments) {}

void Train(Arguments const & arguments)
{
  auto trainerOptions = TrainOptions(arguments.trainConfigPath);

  // load model
  if (!std::filesystem::exists(arguments.modelFolder))
  {
    throw std::runtime_error("Model file does not exist: " + arguments.modelFolder.string());
  }
  auto neuralNetwork = std::make_shared<NeuralNetwork>(arguments.modelFolder);

  // load data
  std::vector<MemoryElement> data;
  DataManager                dataManager = DataManager();
  for (auto const & file: std::filesystem::directory_iterator(arguments.dataFolder))
  {
    if (file.path().extension() == ".bin")
    {
      try
      {
        auto newData = DataManager::LoadGame<MoveTicTacToe>(file.path());
        data.insert(data.end(), newData.begin(), newData.end());
      }
      catch (std::exception const & e)
      {
        LWARN << "Failed to load game from " << file.path().string() << ". Exception: " << e.what();
        throw std::runtime_error("Failed to load game from " + file.path().string() + ". Exception: " + e.what());
      }
    }
  }
  Dataset dataset = Dataset(data);

  // train
  LINFO << "Creating trainer";
  Trainer trainer = Trainer(neuralNetwork);
  LINFO << "Starting training";
  trainer.Train(dataset, trainerOptions);

  // TODO: implement a better way to name trained models
  neuralNetwork->SaveModel(arguments.modelFolder.string() + "_trained");
}

int main(int argc, char ** argv)
{
  // reset random seed
  RandomGenerator::ResetSeed();

  Arguments arguments = ParseArguments(argc, argv);
  auto &    device    = Device::GetInstance(arguments.useCuda);

  switch (arguments.mode)
  {
  case Mode::CREATEMODEL:
  {
    CreateModel(arguments);
    break;
  }
  case Mode::SELFPLAY:
  {
    SelfPlay(arguments);
    break;
  }
  case Mode::EVALUATE:
  {
    Evaluate(arguments);
    break;
  }
  case Mode::TRAIN:
  {
    Train(arguments);
    break;
  }
  }

  return 0;
}
