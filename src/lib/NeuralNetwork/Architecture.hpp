#pragma once

#include <sys/types.h>
struct NetworkArchitecture
{
  uint inputPlanes;    // the amount of planes (channels) the input has
  uint width;          // the width of each plane
  uint height;         // the height of each plane
  uint outputs;        // the amount of policy outputs
  uint filters;        // the amount of convolutional filters each layer
  uint policyFilters;  // the amount of filters in the policy layers
  uint valueFilters;   // the amount of filters in the value layers
  uint residualBlocks; // the amount of residual blocks
};