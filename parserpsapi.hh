#pragma once

#include "fwd_spdlog.hh"

/**
 * In charge of parsing the data received from the FetcherPSAPI.
 */
class ParserPSAPI /* : public QObject*/ {
    // Q_OBJECT
  public:
    explicit ParserPSAPI(spdlog::sinks_init_list sinks);

    void init();

    // signals:
    void parsed();

  public /*slots*/:
    void parse();

  private:
    std::shared_ptr<spdlog::logger> logger_;
};
