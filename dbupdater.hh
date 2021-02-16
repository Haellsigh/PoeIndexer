#pragma once

#include <memory>

#include "apitypes.hh"
#include "fwd_spdlog.hh"

/**
 * In charge of updating the database with the data put in the queues.
 */
class DbUpdater /*: public QObject*/ {
    // Q_OBJECT
  public:
    explicit DbUpdater(spdlog::sinks_init_list sinks);

    void init();

    // signals:
    void updated();

  public /* slots*/:
    void update();

  private:
    std::shared_ptr<spdlog::logger> logger_;
};
