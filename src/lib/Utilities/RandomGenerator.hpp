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

  // Generate a vector of floats from a gamma distribution
  static std::vector<float> SampleFromGamma(size_t size, float alpha, float beta)
  {
    std::gamma_distribution<float> distribution(alpha, beta);
    std::vector<float>             gammaVector;
    for (size_t i = 0; i < size; i++)
    {
      gammaVector.emplace_back(distribution(generator));
    }
    float sum = std::accumulate(gammaVector.begin(), gammaVector.end(), 0.0F);
    for (auto & n: gammaVector)
    {
      n /= sum;
    }
    return gammaVector;
  }

  // Calculate dirichlet noise based on a given vector of probabilities
  static std::vector<float> CalculateDirichletNoise(std::vector<float> const & probabilities, float alpha, float beta, float dirichletFraction)
  {
    std::vector<float> dirichletNoiseVector;

    std::vector<float> noise = SampleFromGamma(probabilities.size(), alpha, beta);
    dirichletNoiseVector.reserve(noise.size());
    for (size_t i = 0; i < noise.size(); i++)
    {
      dirichletNoiseVector.emplace_back(probabilities[i] * (1 - dirichletFraction) + noise[i] * dirichletFraction);
    }

    return dirichletNoiseVector;
  }

  // sample stochastically
  static size_t StochasticSample(std::vector<float> const & probabilities)
  {
    ResetSeed();
    std::discrete_distribution<size_t> distribution(probabilities.begin(), probabilities.end());
    return distribution(generator);
  }

private:
  std::gamma_distribution<float> m_gammaDistribution;
};
