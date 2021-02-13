#include "fetcherpsapi.hh"

#include "sizeunit.hh"

static constexpr const char *baseUrl = "http://www.pathofexile.com/api/public-stash-tabs?id=";

FetcherPSAPI::FetcherPSAPI(/*QObject* parent*/) /*: QObject(parent)*/ {}

void FetcherPSAPI::init(spdlog::sinks_init_list sinks, const std::string &next_change_id,
                        std::shared_ptr<moodycamel::ReaderWriterQueue<std::vector<uint8_t>>> rawDataQueue) {
    mLogger = std::make_shared<spdlog::logger>("fetcher", sinks);
    mLogger->set_level(spdlog::level::trace);

    mRawDataQueue = rawDataQueue;
    /*mAccessManager = new QNetworkAccessManager(this);

    connect(mAccessManager, &QNetworkAccessManager::finished, this,
            &FetcherPSAPI::handleFetched);*/

    mLogger->info("Initialized");

    // mTotalTimer.start();
    fetch(next_change_id);
}

bool FetcherPSAPI::canFetch() {
    // Remove all fetch times more than 1s ago
    auto now = std::chrono::high_resolution_clock::now();
    mFetchTime.erase(
        std::remove_if(mFetchTime.begin(), mFetchTime.end(),
                       [now](const auto &time_point) { return time_point < (now - std::chrono::milliseconds(1200)); }),
        mFetchTime.end());

    if (mFetchTime.size() >= 2) { return false; }
    return true;
}

void FetcherPSAPI::fetch(const std::string &next_change_id) {
    mLastChangeId = next_change_id;
    // Store fetch time for rate limiting
    mFetchTime.push_back(std::chrono::high_resolution_clock::now());

    // Fetch
    mLogger->info("Fetching {}", next_change_id);

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
