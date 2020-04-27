#include <csignal>

#include <QCoreApplication>

#include <readerwriterqueue.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "dbupdater.hh"
#include "fetcherpsapi.hh"
#include "parserpsapi.hh"

void signal_handler(int signal) {
  if (signal == SIGINT) {
    spdlog::info("Terminating app");
    QCoreApplication::quit();
  } else {
    spdlog::critical("Unhandled signal");
  }
}

int main(int argc, char* argv[]) {
  /// Setup logging
  spdlog::set_level(spdlog::level::trace);
  // Create console sink
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::debug);
  console_sink->set_pattern("[%^%=9l%$] [%=7n] %v");
  // Create file sink
  auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/log.txt", 0,
                                                                       0, false, 100);
  file_sink->set_level(spdlog::level::trace);
  file_sink->set_pattern("[%T.%e] [%^%=9l%$] [%=7n] %v");

  spdlog::sinks_init_list sinks = {file_sink, console_sink};

  // Set default logger
  spdlog::set_default_logger(std::make_shared<spdlog::logger>("main", sinks));

  spdlog::info("Starting application");
  // Handle signals
  spdlog::info("Handling signals");
  std::signal(SIGINT, signal_handler);

  // Start core application
  spdlog::info("Starting core application");
  QCoreApplication a(argc, argv);

  auto rawDataQueue = std::make_shared<moodycamel::ReaderWriterQueue<QByteArray>>(1);
  auto stashQueue   = std::make_shared<moodycamel::ReaderWriterQueue<Stash>>(1);

  FetcherPSAPI fetcher;
  ParserPSAPI  parser;
  DbUpdater    updater;

  a.connect(&fetcher, &FetcherPSAPI::fetched, &parser, &ParserPSAPI::parse);
  a.connect(&parser, &ParserPSAPI::parsed, &updater, &DbUpdater::update);

  spdlog::info("Starting threads");
  updater.init(sinks, stashQueue);
  parser.init(sinks, rawDataQueue);
  fetcher.init(sinks, "692825803-707790951-674874738-763885917-729048811", rawDataQueue);

  return a.exec();
}
