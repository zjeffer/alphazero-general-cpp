#pragma once

#include <filesystem>
#include <string>
#include <vector>

enum class Mode
{
  CREATEMODEL,
  SELFPLAY,
  EVALUATE,
  TRAIN
};

struct Arguments
{
  // mode
  Mode mode = Mode::SELFPLAY;

  // model
  std::filesystem::path              modelFolder;  // for creation, selfplay, training
  std::vector<std::filesystem::path> modelFolders; // for evaluation between 2 models

  // config
  std::filesystem::path networkArchitecturePath = "config/architectures/default.jsonc";
  std::filesystem::path agentConfigPath         = "config/agents/default.jsonc";
  std::filesystem::path gameConfigPath          = "config/game/default.jsonc";
  std::filesystem::path trainConfigPath         = "config/train/default.jsonc";

  // misc
  std::filesystem::path dataFolder = "data";
  bool                  useCuda    = false; // for selfplay, I noticed worse performance when using CUDA
};