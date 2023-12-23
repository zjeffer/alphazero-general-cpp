#pragma once

#include <torch/torch.h>

class Device
{

private:
  torch::Device m_device;

public:
  Device(bool useCuda = false);
  ~Device() = default;

  static Device & GetInstance(bool useCuda = false);

  void InitializeCuda(); // loads cuda if available, otherwise loads cpu

  torch::Device     GetDevice();
  torch::DeviceType GetDeviceType();
};