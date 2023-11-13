#pragma once
#include <iomanip>
#include <iostream>

#undef LOG
#include "StdoutSink.hpp"
#include <g3log/g3log.hpp>
#include <g3log/loglevels.hpp>
#include <g3log/logworker.hpp>

// torch also has LOG defined, redefine it
#define LINFO LOG(INFO)
#define LDEBUG LOG(DEBUG)
#define LWARN LOG(WARNING)
#define LFATAL LOG(FATAL)

class Logger
{
public:
  Logger();
  ~Logger();

private:
  std::unique_ptr<g3::LogWorker> m_logWorker;
  std::unique_ptr<StdoutSink>    m_stdoutSink;
};
