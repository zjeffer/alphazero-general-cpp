#pragma once

#include "../../Game.hpp"
#include "../../lib/Agent/Agent.hpp"
#include "../Configuration/Configuration.hpp"
#include "../Logging/Logger.hpp"
#include "../NeuralNetwork/Architecture/Network.hpp"
#include "../NeuralNetwork/Trainer.hpp"
#include "Argument.hpp"
#include "InputParser.hpp"

namespace
{

auto constexpr PARAMETER_HELP = "--help";

auto constexpr PARAMETER_MODE_CREATE   = "create";
auto constexpr PARAMETER_MODE_SELFPLAY = "selfplay";
auto constexpr PARAMETER_MODE_TRAIN    = "train";
auto constexpr PARAMETER_MODE_EVALUATE = "evaluate";

auto constexpr PARAMETER_MODEL                = "--model";
auto constexpr PARAMETER_NETWORK_ARCHITECTURE = "--network-architecture";
auto constexpr PARAMETER_AGENT_CONFIG         = "--agent-config";
auto constexpr PARAMETER_GAME_CONFIG          = "--game-config";
auto constexpr PARAMETER_TRAIN_CONFIG         = "--train-config";
auto constexpr PARAMETER_DATA_FOLDER          = "--data-folder";
auto constexpr PARAMETER_CUDA                 = "--cuda";

auto constexpr PARAMETER_MODEL1 = "--model1";
auto constexpr PARAMETER_MODEL2 = "--model2";

void PrintHelpMessage(std::string const & programName)
{
  std::cout << "\nUsage: " << programName << " <mode> <options>"
            << "  " << PARAMETER_HELP << "                           Print this help message\n\n"
            << "Modes: (choose one)\n"
            << "  " << PARAMETER_MODE_CREATE << "       Create mode: create a new model\n"
            << "  " << PARAMETER_MODE_SELFPLAY << "     Self-play mode: play games using a given model\n"
            << "  " << PARAMETER_MODE_TRAIN << "        Train mode: train a given model\n"
            << "  " << PARAMETER_MODE_EVALUATE << "     Evaluate mode: evaluate two given models\n"
            << "Model folder\n"
            << "  " << PARAMETER_MODEL << " <path>                  Path to the model folder (for create, selfplay and training)\n"
            << "  " << PARAMETER_MODEL1 << " <path>                 Path to the first model folder (for evaluation)\n"
            << "  " << PARAMETER_MODEL2 << " <path>                 Path to the second model folder (for evaluation)\n"
            << "Config files\n"
            << "  " << PARAMETER_NETWORK_ARCHITECTURE << " <path>   Path to the network architecture to load\n"
            << "  " << PARAMETER_AGENT_CONFIG << " <path>           Path to the agent configuration to load\n"
            << "  " << PARAMETER_GAME_CONFIG << " <path>            Path to the game configuration to load\n"
            << "  " << PARAMETER_TRAIN_CONFIG << " <path>           Path to the training configuration to load\n"
            << "Misc\n"
            << "  " << PARAMETER_DATA_FOLDER << " <path>            Path to the folder where games are loaded/stored\n"
            << "  " << PARAMETER_CUDA << "                          Use CUDA (GPU) if available (default: false)";
  std::cout << std::endl;
}

auto constexpr MODEL_FILENAME        = "model.pt";
auto constexpr ARCHITECTURE_FILENAME = "model.jsonc";

} // namespace

Mode GetMode(InputParser const & input)
{
  if (input.CmdOptionExists(PARAMETER_MODE_CREATE))
  {
    return Mode::CREATEMODEL;
  }
  if (input.CmdOptionExists(PARAMETER_MODE_SELFPLAY))
  {
    return Mode::SELFPLAY;
  }
  if (input.CmdOptionExists(PARAMETER_MODE_TRAIN))
  {
    return Mode::TRAIN;
  }
  if (input.CmdOptionExists(PARAMETER_MODE_EVALUATE))
  {
    return Mode::EVALUATE;
  }
  throw std::runtime_error("No mode specified.");
}

void VerifyModelExists(std::filesystem::path const & model, std::filesystem::path const & networkArchitecturePath)
{
  if (!std::filesystem::exists(model))
  {
    throw std::runtime_error("Model file does not exist: " + model.string());
  }
  if (networkArchitecturePath.empty())
  {
    throw std::runtime_error("Missing network architecture file");
  }
  if (!std::filesystem::exists(networkArchitecturePath))
  {
    throw std::runtime_error("Network architecture file does not exist: " + networkArchitecturePath.string());
  }
}

void GetModel(InputParser const & input, Arguments & arguments, bool checkIfExists = true)
{
  if (input.CmdOptionExists(PARAMETER_MODEL))
  {
    // the model folder contains the model and the network architecture
    arguments.modelFolder = std::filesystem::path(input.GetCmdOption(PARAMETER_MODEL));
  }
  else
  {
    throw std::runtime_error("Missing model argument");
  }
  if (!std::filesystem::exists(arguments.modelFolder))
  {
    std::filesystem::create_directories(arguments.modelFolder);
  }
  else if (!std::filesystem::is_directory(arguments.modelFolder))
  {
    throw std::runtime_error("Model folder needs to be a directory: " + arguments.modelFolder.string());
  }

  if (input.CmdOptionExists(PARAMETER_NETWORK_ARCHITECTURE))
  {
    arguments.networkArchitecturePath = input.GetCmdOption(PARAMETER_NETWORK_ARCHITECTURE);
  }
  else
  {
    arguments.networkArchitecturePath = arguments.modelFolder / ARCHITECTURE_FILENAME;
  }

  // if we are not creating a new model, check if the model exists and throw an error if it doesn't
  if (checkIfExists)
  {
    VerifyModelExists(arguments.modelFolder / MODEL_FILENAME, arguments.networkArchitecturePath);
  }
}

void GetAgentConfig(InputParser const & input, Arguments & arguments)
{
  if (input.CmdOptionExists(PARAMETER_AGENT_CONFIG))
  {
    arguments.agentConfigPath = input.GetCmdOption(PARAMETER_AGENT_CONFIG);
  }
}

void GetGameConfig(InputParser const & input, Arguments & arguments)
{
  if (input.CmdOptionExists(PARAMETER_GAME_CONFIG))
  {
    arguments.gameConfigPath = input.GetCmdOption(PARAMETER_GAME_CONFIG);
  }
}

void GetDataFolder(InputParser const & input, Arguments & arguments, bool checkIfExists = false)
{
  // folder where games are stored
  if (input.CmdOptionExists(PARAMETER_DATA_FOLDER))
  {
    arguments.dataFolder = input.GetCmdOption(PARAMETER_DATA_FOLDER);
  }
  if (arguments.dataFolder.empty())
  {
    throw std::runtime_error("Missing data folder");
  }
  if (!std::filesystem::exists(arguments.dataFolder))
  {
    if (checkIfExists)
    {
      throw std::runtime_error("Data folder does not exist: " + arguments.dataFolder.string());
    }
    std::filesystem::create_directories(arguments.dataFolder);
  }
}

void GetTrainConfig(InputParser const & input, Arguments & arguments)
{
  if (input.CmdOptionExists(PARAMETER_TRAIN_CONFIG))
  {
    arguments.trainConfigPath = input.GetCmdOption(PARAMETER_TRAIN_CONFIG);
  }
  if (arguments.trainConfigPath.empty())
  {
    throw std::runtime_error("Missing training configuration file");
  }
  if (!std::filesystem::exists(arguments.trainConfigPath))
  {
    throw std::runtime_error("Training configuration file does not exist: " + arguments.trainConfigPath.string());
  }
  if (!std::filesystem::exists(arguments.dataFolder))
  {
    throw std::runtime_error("Data folder does not exist: " + arguments.dataFolder.string());
  }
  if (!std::filesystem::is_directory(arguments.dataFolder))
  {
    throw std::runtime_error("Data folder is not a directory: " + arguments.dataFolder.string());
  }
}

void GetDeviceOptions(InputParser const & input, Arguments & arguments)
{
  if (input.CmdOptionExists(PARAMETER_CUDA))
  {
    arguments.useCuda = true;
  }
}

void GetEvaluationModels(InputParser const & input, Arguments & arguments)
{
  if (input.CmdOptionExists(PARAMETER_MODEL1) && input.CmdOptionExists(PARAMETER_MODEL2))
  {
    arguments.modelFolders = {input.GetCmdOption(PARAMETER_MODEL1), input.GetCmdOption(PARAMETER_MODEL2)};
  }
  else
  {
    throw std::runtime_error("Missing two models for evaluation");
  }
  for (auto const & modelFolder: arguments.modelFolders)
  {
    if (!std::filesystem::exists(modelFolder))
    {
      throw std::runtime_error("Model file does not exist: " + modelFolder.string());
    }
    if (!std::filesystem::exists(modelFolder / MODEL_FILENAME))
    {
      throw std::runtime_error("Network file does not exist: " + (modelFolder / MODEL_FILENAME).string());
    }
    if (!std::filesystem::exists(modelFolder / ARCHITECTURE_FILENAME))
    {
      throw std::runtime_error("Network architecture file does not exist: " + (modelFolder / ARCHITECTURE_FILENAME).string());
    }
  }
}

Arguments ParseArguments(int argc, char ** argv)
{
  LDEBUG << "Parsing " << argc << " arguments";
  Arguments   arguments;
  InputParser input(argc, argv);

  if (input.CmdOptionExists(PARAMETER_HELP))
  {
    PrintHelpMessage(argv[0]);
    exit(0);
  }

  try
  {
    arguments.mode = GetMode(input);
    GetDeviceOptions(input, arguments);

    // training
    switch (arguments.mode)
    {
    case Mode::CREATEMODEL:
    {
      GetModel(input, arguments, false); // don't check if model exists
      break;
    }
    case Mode::SELFPLAY:
    {
      GetModel(input, arguments);
      GetAgentConfig(input, arguments);
      GetGameConfig(input, arguments);
      GetDataFolder(input, arguments);
      break;
    }
    case Mode::TRAIN:
    {
      GetModel(input, arguments);
      GetAgentConfig(input, arguments);
      GetDataFolder(input, arguments, true);
      GetTrainConfig(input, arguments);
      break;
    }
    case Mode::EVALUATE:
    {
      if (input.CmdOptionExists(PARAMETER_MODEL1) && input.CmdOptionExists(PARAMETER_MODEL2))
      {
        arguments.modelFolders = {input.GetCmdOption(PARAMETER_MODEL1), input.GetCmdOption(PARAMETER_MODEL2)};
      }
      else
      {
        throw std::runtime_error("Missing two models for evaluation");
      }
      break;
    }
    }
  }
  catch (std::exception const & e)
  {
    std::cerr << "Failed to parse arguments: " << e.what();
    std::cout << std::endl;
    PrintHelpMessage(argv[0]);
    exit(1);
  }

  return arguments;
}
