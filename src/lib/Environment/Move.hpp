#pragma once

#include <sys/types.h>

#include <string>
#include <utility>

class Move
{
public:
  virtual ~Move() = default;

  virtual std::pair<uint, uint> GetCoordinates() const                      = 0;
  virtual uint                  GetRow() const                              = 0;
  virtual uint                  GetColumn() const                           = 0;
  virtual float                 GetPriorProbability() const                 = 0;
  virtual void                  SetPriorProbability(float priorProbability) = 0;
  virtual std::string           ToString() const                            = 0;
  virtual size_t                GetIndex() const                            = 0;
};