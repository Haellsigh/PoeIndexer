#include <span>

#include <cpr/cpr.h>
#include <simdjson.h>
#include <spdlog/spdlog.h>

#include "fetcherpsapi.hh"
#include "sizeunit.hh"

static constexpr const char *base_fetch_url = "http://www.pathofexile.com/api/public-stash-tabs?id=";

FetcherPSAPI::FetcherPSAPI(spdlog::sinks_init_list sinks)
    : logger_(std::make_shared<spdlog::logger>("fetcher", sinks)) {
    logger_->set_level(spdlog::level::trace);

    /*mAccessManager = new QNetworkAccessManager(this);

    connect(mAccessManager, &QNetworkAccessManager::finished, this,
            &FetcherPSAPI::handleFetched);*/
    logger_->info("Initialized");
}

void FetcherPSAPI::init(const std::string &next_change_id) { fetch(next_change_id); }

bool FetcherPSAPI::canFetch() {
    // Remove all fetch times more than 1s ago
    auto now = std::chrono::high_resolution_clock::now();
    mFetchTime.erase(
        std::remove_if(mFetchTime.begin(), mFetchTime.end(),
                       [now](const auto &time_point) { return time_point < (now - std::chrono::milliseconds(1200)); }),
        mFetchTime.end());

    // Can't fetch if we have more than two requests in the last second
    if (mFetchTime.size() >= 2) { return false; }
    return true;
}

void FetcherPSAPI::fetch(const std::string &next_change_id) {
    mLastChangeId = next_change_id;
    // Store fetch time for rate limiting
    mFetchTime.push_back(std::chrono::high_resolution_clock::now());

    // Fetch
    logger_->info("Fetching {}", next_change_id);

    const cpr::Url url{base_fetch_url + next_change_id};

    const auto id = request_id_++;
    responses_.emplace(
        id, request{cpr::GetCallback(
                [=](cpr::Response r) -> void {
                    auto &&data = responses_[id].data;

                    logger_->debug("Finished request #{} with code {}", id, r.status_code);
                    if (r.status_code != 200) {
                        const auto &limit = r.header.at("X-Rate-Limit-Ip-State");
                        // limit is formatted this way:
                        // {requests}:{window}:{throttled}
                        // {x}:       {y}:     {z}
                        // x requests per y seconds, throttled by z seconds
                        // The part after the last ':' always represent the amount of seconds to wait
                        // before doing the next request
                        const auto throttled_str = limit.substr(limit.rfind(':') + 1);
                        const auto throttled     = std::stoi(throttled_str);

                        logger_->debug("We are throttled for {}s", throttled);
                    } else {
                        logger_->debug("size: {}", data.size());
                        // TODO MOVE TO PARSERAPI
                        // simple parse
                        using namespace simdjson;

                        dom::parser parser;
                        dom::object object;
                        error_code error;

                        parser.parse(padded_string(data.data(), data.size())).get<dom::object>().tie(object, error);

                        if (error || object.size() != 2) {
                            logger_->error("Invalid data received");
                            return;
                        }

                        int nStashes = 0, nStashesCurrentLeague = 0, nItems = 0, nItemsCurrentLeague = 0;

                        dom::element stashes;
                        object["stashes"].tie(stashes, error);

                        if (error || !stashes.is<dom::array>()) {
                            logger_->error("Could not find stashes data in json");
                            return;
                        }

                        for (const auto &stash : stashes) {
                            nStashes++;

                            std::string_view league;
                            stash["league"].get<std::string_view>().tie(league, error);

                            if (error) {
                                // No league data
                                // mLogger->warn("Json error parsing league: {}", error);
                                continue;
                            }

                            dom::array items;
                            stash["items"].get<dom::array>().tie(items, error);

                            if (error) {
                                logger_->error("Json error parsing items: {}", error);
                                continue;
                            }

                            nItems += items.size();

                            if (league == std::string_view("Ritual")) {
                                nStashesCurrentLeague++;
                                nItemsCurrentLeague += items.size();
                                // mLogger->trace("Account {}, stash id {}", stash["accountName"], stash["id"]);
                            } else {
                            }
                        }

                        logger_->debug("Parsed {}/{} stashes for {}/{} items", nStashesCurrentLeague, nStashes,
                                       nItemsCurrentLeague, nItems);
                        n_items_ += nItems;
                    }

                    logger_->debug("Total items: {}", n_items_);

                    responses_.erase(id);
                },
                url, cpr::WriteCallback([=](const std::string &data) -> bool {
                    handle_new_data(id, data);
                    return true;
                }))});

    /*QNetworkRequest req(baseUrl + next_change_id);
    req.setRawHeader(
        "User-Agent",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:75.0) Gecko/20100101 Firefox/75.0");

    auto reply = mAccessManager->get(req);
    mReplies.insert(reply, {});*/

    // Fetches next_change_id before the download is complete
    /*connect(reply, &QNetworkReply::readyRead, [&, reply]() {
      handleNewData(reply, true);*/
    /*
    auto&& stashReply = mReplies[reply];
    auto&& data       = stashReply.data;
    data.append(reply->readAll());

    // If we're rate limited, don't fetch next request yet
    if (!checkRateLimits(reply))
      return;

    if (!stashReply.fetchingNextChangeId && data.contains(',')) {
      auto next_change_id = extractNextChangeId(data);
      if (next_change_id != mLastChangeId) {
        stashReply.fetchingNextChangeId = true;
        while (!canFetch()) {
          mLogger->warn("Waiting 500ms to avoid rate limiting");
          std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        fetch(next_change_id);
      }
    }
    */
    /*});*/
}

void FetcherPSAPI::handle_new_data(std::size_t id, const std::string &data) {
    auto &&r = responses_[id];

    r.data.append(data);
    if (r.found_next_change_id) { return; }

    if (data.size() < 50) return;
    // logger_->debug("Handling data for request #{}", id);
    auto start = std::find(data.begin(), data.end(), ':');
    if (start >= std::prev(data.end(), 2)) return;
    std::advance(start, 2);
    auto end = std::find(start, data.end(), ',');
    if (end == data.end()) return;
    std::advance(end, -1);
    if (end <= start) return;

    std::string_view nci(start, end);
    if (std::count(nci.begin(), nci.end(), '-') != 4) return;
    // logger_->debug("next change id: {}", nci);

    r.found_next_change_id = true;

    fetch(std::string(nci));
}

/*
void FetcherPSAPI::handleNewData(QNetworkReply* reply,
                                 bool           stopOnRateLimiting) {
  auto&& stashReply = mReplies[reply];
  auto&& data       = stashReply.data;
  data.append(reply->readAll());

  if (stashReply.fetchingNextChangeId) {
    return;  // already fetching
  }

  if (!checkRateLimits(reply)) {
    if (stopOnRateLimiting) {
      return;
    } else {
      waitRateLimitingTimeout(reply);
    }
  }

  auto next_change_id = extractNextChangeId(data);
  // Checks for valid next_change_id
  if (next_change_id.count('-' != 4)) {
    mLogger->error("Invalid next_change_id in reply: {}", data.left(100));
    mLogger->warn("Possible causes: API rate limiting or internet connection error");
    return;
  }

  // Mark next_change_id as fetched already
  stashReply.fetchingNextChangeId = true;

  // Avoid rate limiting as much as possible
  if (!canFetch())
    mLogger->trace("Throttling fetch to avoid rate limiting");
  while (!canFetch()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  fetch(next_change_id);
}

void FetcherPSAPI::handleFetched(QNetworkReply* reply) {
  handleNewData(reply);

  // Process data asap
  auto&& stashReply = mReplies[reply];
  mRawDataQueue->enqueue(stashReply.data);
  emit fetched();

  // Cleanup
  mReplies.remove(reply);
  reply->deleteLater();
}

QString FetcherPSAPI::extractNextChangeId(const QByteArray& data) {
  return data.left(data.indexOf(',') - 1).mid(data.indexOf(':') + 2);
}

bool FetcherPSAPI::checkRateLimits(QNetworkReply* reply) {
  // X-Rate-Limit-Ip-State    0:1:timeout
  auto tokens  = reply->rawHeader("X-Rate-Limit-Ip-State").split(':');
  bool ok      = false;
  int  timeout = tokens.at(2).toInt(&ok);
  if (ok && timeout == 0) {
    return true;
  } else if (!ok) {
    mLogger->warn("Wrong header content: {}",
                  reply->rawHeader("X-Rate-Limit-Ip-State").toStdString());
  }
  return false;
}

void FetcherPSAPI::waitRateLimitingTimeout(QNetworkReply* reply) {
  // X-Rate-Limit-Ip-State    0:1:timeout
  auto tokens  = reply->rawHeader("X-Rate-Limit-Ip-State").split(':');
  bool ok      = false;
  int  timeout = tokens.at(2).toInt(&ok);
  if (ok && timeout != 0) {
    mLogger->warn("We are rate limited, waiting for {} seconds", timeout);
    std::this_thread::sleep_for(std::chrono::seconds(timeout));
  }
}
*/
