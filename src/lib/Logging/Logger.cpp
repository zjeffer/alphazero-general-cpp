#include "Logger.hpp"

#include <memory>

#include "StdoutSink.hpp"
#include <g3log/filesink.hpp>
#include <g3log/loglevels.hpp>

Logger::Logger()
  : m_logWorker(g3::LogWorker::createLogWorker())
  , m_stdoutSink(std::make_unique<StdoutSink>())
{
  // log to stdout
  m_logWorker->addSink(std::move(m_stdoutSink), &StdoutSink::Callback);
  // log to file
  m_logWorker->addSink(std::make_unique<g3::FileSink>("alphazero", "logs"), &g3::FileSink::fileWrite);

  g3::initializeLogging(m_logWorker.get());
}

Logger::~Logger()
{
  m_logWorker->removeAllSinks();
  m_logWorker.reset();
}