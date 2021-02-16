#pragma once

#include <chrono>
#include <future>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <cpr/cpr.h>

#include "fwd_spdlog.hh"

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
    explicit FetcherPSAPI(spdlog::sinks_init_list sinks);

    void init(const std::string &next_change_id);

    // signals:
    void fetched();

  private:
    bool canFetch();
    void fetch(const std::string &next_change_id);

    void handle_new_data(std::size_t id, const std::string &data);

  private:
    std::shared_ptr<spdlog::logger> logger_;
    std::size_t request_id_ = 0;

    std::size_t next_request_id = 0;
    // QHash<QNetworkReply*, StashAPIReply> mReplies;
    std::unordered_map<std::size_t, std::future<void>> responses_;
    std::string mLastChangeId;

    std::vector<std::chrono::high_resolution_clock::time_point> mFetchTime;
};
