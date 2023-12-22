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

void SelfPlay(Arguments const & arguments)
{
  NetworkArchitecture architecture = LoadArchitecture(arguments.networkArchitecturePath);
  AgentOptions        agentOptions = LoadAgentOptions(arguments.agentConfigPath);
  GameOptions         gameOptions  = LoadGameOptions(arguments.gameConfigPath);

  // load or create model
  std::shared_ptr<NeuralNetwork> neuralNetwork;
  if (std::filesystem::exists(arguments.modelPath))
  {
    neuralNetwork = std::make_unique<NeuralNetwork>(architecture, arguments.modelPath);
  }
  else
  {
    // check if modelPath is writable
    std::ofstream file(arguments.modelPath);
    if (!file)
    {
      throw std::runtime_error("Model file destination is not writable: " + arguments.modelPath.string());
    }
    file.close();
    neuralNetwork = std::make_unique<NeuralNetwork>(architecture);
    neuralNetwork->SaveModel(arguments.modelPath);
  }

  // create agents
  std::vector<std::shared_ptr<Agent>> agents;
  agents.reserve(agentOptions.agentNames.size());
  for (auto const & agentName: agentOptions.agentNames)
  {
    agents.emplace_back(std::make_unique<Agent>(agentName, neuralNetwork));
  }

  while (true)
  {
    Game game = Game(std::make_unique<EnvironmentTicTacToe>(), agents, gameOptions);
    game.PlayGame();
  }
}

void Train(Arguments const & arguments)
{
  NetworkArchitecture architecture   = LoadArchitecture(arguments.networkArchitecturePath);
  TrainOptions        trainerOptions = LoadTrainOptions(arguments.trainConfigPath);

  // load model
  if (!std::filesystem::exists(arguments.modelPath))
  {
    throw std::runtime_error("Model file does not exist: " + arguments.modelPath.string());
  }
  auto neuralNetwork = std::make_shared<NeuralNetwork>(architecture, arguments.modelPath);

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
  neuralNetwork->SaveModel(std::string(arguments.modelPath.parent_path() / arguments.modelPath.stem().string()) + "_trained"
                           + arguments.modelPath.extension().string());
}

int main(int argc, char ** argv)
{
  // reset random seed
  RandomGenerator::ResetSeed();

  Arguments arguments = ParseArguments(argc, argv);
  auto      device    = Device(arguments.useCuda);

  if (arguments.train)
  {
    Train(arguments);
  }
  else
  {
    SelfPlay(arguments);
  }

  return 0;
}
