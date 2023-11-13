#include "StdoutSink.hpp"

void StdoutSink::Callback(g3::LogMessageMover log)
{
    std::cout << log.get().toString(&CustomSink::FormatMsg) << std::flush;
}
