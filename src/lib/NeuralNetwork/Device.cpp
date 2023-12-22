#include "Device.hpp"

#include "../Logging/Logger.hpp"

Device::Device(bool useCuda)
  : m_device(torch::kCPU)
{
  if (useCuda)
  {
    InitializeCuda();
  }
}

Device & Device::GetInstance()
{
  static Device instance;
  return instance;
}

void Device::InitializeCuda()
{
  if (torch::cuda::is_available())
  {
    try
    {
      m_device = torch::Device(torch::kCUDA);
    }
    catch (std::exception const & e)
    {
      LWARN << "Error initializing CUDA: " << e.what();
    }
  }
  else
  {
    LWARN << "CUDA is not available. Using CPU instead.";
  }
}

torch::Device Device::GetDevice()
{
  return m_device;
}

torch::DeviceType Device::GetDeviceType()
{
  return m_device.type();
}
