#pragma once
#include <random>

class RandomGenerator
{
public:
  static std::mt19937 generator;

  RandomGenerator() = delete;

  // Reset the seed of the random generator
  static void ResetSeed()
  {
    generator.seed(std::random_device{}());
  }

  // Generate a random number between min and max (inclusive)
  static int GenerateRandomNumber(int min, int max)
  {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
  }

  // Generate a float from a gamma distribution
  static float GenerateGamma(float alpha, float beta)
  {
    std::gamma_distribution<float> distribution(alpha, beta);
    return distribution(generator);
  }

private:
  std::gamma_distribution<float> m_gammaDistribution;
};
