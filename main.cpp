#include <csignal>
#include <memory>

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "dbupdater.hh"
#include "fetcherpsapi.hh"
#include "parserpsapi.hh"

bool stop_application_g = false;

void signal_handler(int signal);

/**
 * This application has 3 workers working concurrently:
 *  - FetchWorker
 *  - ParserWorker
 *  - DbUpdaterWorker
 *
 *  FetchWorker:
 *      Fetches the latest json data from the official api.
 *      It tries to parse the beginning of a file until it finds the "next_change_id", then use that value to start the
 *      next fetch.
 *      Once a json file is fully downloaded, it passes on the json data to the ParserWorker.
 *
 *  ParserWorker:
 *      When it gets
 */
int main(int argc, char *argv[]) {
    /// Setup logging
    spdlog::set_level(spdlog::level::trace);
    // Create console sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);
    console_sink->set_pattern("[%^%=9l%$] [%=7n] %v");
    // Create file sink
    auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/log.txt", 0, 0, false, 100);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%T.%e] [%^%=9l%$] [%=7n] %v");

    spdlog::sinks_init_list sinks = {file_sink, console_sink};

    // Set default logger
    spdlog::set_default_logger(std::make_shared<spdlog::logger>("main", sinks));

    spdlog::info("Starting application");
    std::signal(SIGTERM, signal_handler);
    std::signal(SIGSEGV, signal_handler);
    std::signal(SIGINT, signal_handler);
    std::signal(SIGILL, signal_handler);
    std::signal(SIGABRT, signal_handler);
    std::signal(SIGFPE, signal_handler);

    FetcherPSAPI fetcher(sinks);
    ParserPSAPI parser(sinks);
    DbUpdater updater(sinks);

    // a.connect(&fetcher, &FetcherPSAPI::fetched, &parser, &ParserPSAPI::parse);
    // a.connect(&parser, &ParserPSAPI::parsed, &updater, &DbUpdater::update);

    spdlog::info("Starting threads");
    updater.init();
    parser.init();
    fetcher.init("1058098813-1067130919-1025852952-1152855130-1105707904");

    while (!stop_application_g)
        ;

    // return a.exec();
    return 0;
}

void signal_handler(int signal) {
    if (signal == SIGINT) {
        spdlog::info("Terminating app");
    } else {
        spdlog::critical("Unhandled signal #{}", signal);
    }

    stop_application_g = true;
}
