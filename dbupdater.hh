#pragma once

#include <moodycamel/readerwriterqueue.h>
#include <spdlog/spdlog.h>

#include "apitypes.hh"

/**
 * In charge of updating the database with the data put in the queues.
 */
class DbUpdater /*: public QObject*/ {
    // Q_OBJECT
  public:
    explicit DbUpdater(/*QObject* parent = nullptr*/);

    void init(spdlog::sinks_init_list sinks, std::shared_ptr<moodycamel::ReaderWriterQueue<Stash>> stashQueue);

    // signals:
    void updated();

  public /* slots*/:
    void update();

  private:
    std::shared_ptr<spdlog::logger> mLogger;

    std::shared_ptr<moodycamel::ReaderWriterQueue<Stash>> mStashQueue;
};
