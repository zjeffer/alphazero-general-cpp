#pragma once

#include "CustomSink.hpp"

class StdoutSink : public CustomSink
{
  public:
    void Callback(g3::LogMessageMover log) override;
};