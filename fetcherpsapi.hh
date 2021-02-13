#pragma once

#include <memory>

#include <moodycamel/readerwriterqueue.h>
#include <spdlog/spdlog.h>

struct StashAPIReply {
    StashAPIReply() { /*timer.start();*/
    }

    bool fetchingNextChangeId = false;
    std::vector<uint8_t> data;
    // QElapsedTimer timer;
};

/**
 * In charge of fetching the latest data from the public stash api.
 */
class FetcherPSAPI /* : public QObject */ {
    // Q_OBJECT
  public:
    explicit FetcherPSAPI(/*QObject* parent = nullptr*/);

    void init(spdlog::sinks_init_list sinks, const std::string &next_change_id,
              std::shared_ptr<moodycamel::ReaderWriterQueue<std::vector<uint8_t>>> rawDataQueue);

    // signals:
    void fetched();

  private:
    bool canFetch();
    void fetch(const std::string &next_change_id);

    // void handleNewData(QNetworkReply* reply, bool stopOnRateLimiting = false);

    // void handleFetched(QNetworkReply* reply);

    // std::string extractNextChangeId(const std::vector<uint8_t>& data);
    // bool    checkRateLimits(QNetworkReply* reply);
    // void    waitRateLimitingTimeout(QNetworkReply* reply);

  private:
    std::shared_ptr<spdlog::logger> mLogger;
    // QHash<QNetworkReply*, StashAPIReply> mReplies;
    std::string mLastChangeId;

    std::shared_ptr<moodycamel::ReaderWriterQueue<std::vector<uint8_t>>> mRawDataQueue;

    // QElapsedTimer mTotalTimer;
    double mTotalSize = 0;

    std::vector<std::chrono::high_resolution_clock::time_point> mFetchTime;
};
