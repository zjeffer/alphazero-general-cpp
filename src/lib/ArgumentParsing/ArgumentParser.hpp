#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "../../Game.hpp"
#include "../../lib/Agent/Agent.hpp"
#include "../Configuration/Configuration.hpp"
#include "../Logging/Logger.hpp"
#include "../NeuralNetwork/Architecture/Network.hpp"
#include "../NeuralNetwork/Trainer.hpp"

// argument parsing (https://stackoverflow.com/a/868894/10180569)
class InputParser
{
public:
  InputParser(int & argc, char ** argv)
  {
    for (int i = 1; i < argc; ++i)
    {
      m_tokens.emplace_back(argv[i]);
    }
  }

  /// @author iain
  std::string const & GetCmdOption(std::string const & option) const
  {
    std::vector<std::string>::const_iterator itr;
    itr = std::find(m_tokens.begin(), m_tokens.end(), option);
    if (itr != m_tokens.end() && ++itr != m_tokens.end())
    {
      return *itr;
    }
    static const std::string emptyString("");
    return emptyString;
  }

  /// @author iain
  bool CmdOptionExists(std::string const & option) const
  {
    return std::find(m_tokens.begin(), m_tokens.end(), option) != m_tokens.end();
  }

private:
  std::vector<std::string> m_tokens;
};

struct Arguments
{
  std::filesystem::path modelPath               = "models/model.pt";
  std::filesystem::path networkArchitecturePath = "config/architectures/default.jsonc";
  std::filesystem::path agentConfigPath         = "config/agents/default.jsonc";
  std::filesystem::path gameConfigPath          = "config/game/default.jsonc";
  // training
  bool                  train           = false;
  std::filesystem::path trainConfigPath = "config/train/default.jsonc";
  std::filesystem::path dataFolder      = "data";
  // device
  bool useCuda = false; // for selfplay, I noticed worse performance when using CUDA
};

static auto constexpr HELP_MESSAGE = R"(
Usage: %s [options]
  --help                          Print this help message
  --model <path>                  Path to the model to load
  --network-architecture <path>   Path to the network architecture to load
  --agent-config <path>           Path to the agent configuration to load
  --game-config <path>            Path to the game configuration to load
  --train                         Train mode: train a given model
  --train-config <path>           Path to the training configuration to load
  --data-folder <path>            Path to the folder where games are stored
  --cuda                          Use CUDA (GPU) if available (default: false)
)";

Arguments ParseArguments(int argc, char ** argv)
{
  LDEBUG << "Parsing " << argc << " arguments";
  Arguments   arguments;
  InputParser input(argc, argv);

  if (input.CmdOptionExists("--help"))
  {
    printf(HELP_MESSAGE, argv[0]);
    exit(0);
  }

  try
  {
    if (input.CmdOptionExists("--model"))
    {
      arguments.modelPath = input.GetCmdOption("--model");
    }
    if (input.CmdOptionExists("--game-config"))
    {
      arguments.gameConfigPath = input.GetCmdOption("--game-config");
    }
    if (input.CmdOptionExists("--network-architecture"))
    {
      arguments.networkArchitecturePath = input.GetCmdOption("--network-architecture");
    }
    if (input.CmdOptionExists("--agent-config"))
    {
      arguments.agentConfigPath = input.GetCmdOption("--agent-config");
    }

    // training
    if (input.CmdOptionExists("--train"))
    {
      arguments.train = true;
      if (input.CmdOptionExists("--train-config"))
      {
        arguments.trainConfigPath = input.GetCmdOption("--train-config");
      }
      if (arguments.trainConfigPath.empty())
      {
        throw std::runtime_error("Missing training configuration file");
      }
      if (!std::filesystem::exists(arguments.trainConfigPath))
      {
        throw std::runtime_error("Training configuration file does not exist: " + arguments.trainConfigPath.string());
      }

      // folder where games are stored
      if (input.CmdOptionExists("--data-folder"))
      {
        arguments.dataFolder = input.GetCmdOption("--data-folder");
        if (!std::filesystem::exists(arguments.dataFolder))
        {
          throw std::runtime_error("Data folder does not exist: " + arguments.dataFolder.string());
        }
        if (!std::filesystem::is_directory(arguments.dataFolder))
        {
          throw std::runtime_error("Data folder is not a directory: " + arguments.dataFolder.string());
        }
      }
    }

    // device
    if (input.CmdOptionExists("--cuda"))
    {
      arguments.useCuda = true;
    }
  }
  catch (std::exception const & e)
  {
    std::cerr << "Failed to parse arguments: " << e.what();
    std::cout << std::endl;
    printf(HELP_MESSAGE, argv[0]);
    exit(1);
  }

  return arguments;
}

NetworkArchitecture LoadArchitecture(std::filesystem::path const & file)
{
  LDEBUG << "Loading network architecture from " << file;
  auto networkArchitectureConfig = Configuration(file);
  return NetworkArchitecture{
    .width                  = networkArchitectureConfig.Get<uint>("board.width"),
    .height                 = networkArchitectureConfig.Get<uint>("board.height"),
    .inputPlanes            = networkArchitectureConfig.Get<uint>("network_architecture.input_planes"),
    .residualBlocks         = networkArchitectureConfig.Get<uint>("network_architecture.residual_blocks"),
    .filters                = networkArchitectureConfig.Get<uint>("network_architecture.filters"),
    .policyOutputs          = networkArchitectureConfig.Get<uint>("network_architecture.policy_outputs"),
    .policyFilters          = networkArchitectureConfig.Get<uint>("network_architecture.policy_filters"),
    .valueFilters           = networkArchitectureConfig.Get<uint>("network_architecture.value_filters"),
    .valueHeadLinearNeurons = networkArchitectureConfig.Get<uint>("network_architecture.value_head_linear_neurons"),
    .kernelSize             = networkArchitectureConfig.Get<uint>("network_architecture.kernel.size"),
    .padding                = networkArchitectureConfig.Get<uint>("network_architecture.kernel.padding"),
    .stride                 = networkArchitectureConfig.Get<uint>("network_architecture.kernel.stride"),
  };
}

GameOptions LoadGameOptions(std::filesystem::path const & file)
{
  LDEBUG << "Loading game options from " << file;
  auto gameOptionsConfig = Configuration(file);
  return GameOptions{
    .saveMemory = gameOptionsConfig.Get<bool>("save_memory"),
    .maxMoves   = gameOptionsConfig.Get<uint>("max_moves"),
    .simsPerMove = gameOptionsConfig.Get<uint>("sims_per_move"),
    .stochasticSearch = gameOptionsConfig.Get<bool>("stochastic_search"),
    .dirichletNoiseOptions = DirichletNoiseOptions{
      .enable = gameOptionsConfig.Get<bool>("dirichlet_noise.enable"),
      .alpha = gameOptionsConfig.Get<float>("dirichlet_noise.alpha"),
      .beta  = gameOptionsConfig.Get<float>("dirichlet_noise.beta"),
      .dirichletFraction = gameOptionsConfig.Get<float>("dirichlet_noise.dirichlet_fraction"),
    },
  };
}

AgentOptions LoadAgentOptions(std::filesystem::path const & file)
{
  LDEBUG << "Loading agent options from " << file;
  auto agentOptionsConfig = Configuration(file);
  return AgentOptions{
    .agentNames = agentOptionsConfig.Get<std::vector<std::string>>("agent_names"),
  };
}

TrainOptions LoadTrainOptions(std::filesystem::path const & file)
{
  LDEBUG << "Loading training options from " << file;
  auto trainOptionsConfig = Configuration(file);
  return TrainOptions{
    .batchSize    = trainOptionsConfig.Get<uint>("batch_size"),
    .epochs       = trainOptionsConfig.Get<uint>("epochs"),
    .learningRate = trainOptionsConfig.Get<float>("learning_rate"),
  };
}