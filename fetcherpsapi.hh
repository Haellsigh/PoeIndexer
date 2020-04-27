#pragma once

#include <memory>

#include <QElapsedTimer>
#include <QHash>
#include <QObject>
#include <QPair>

#include <readerwriterqueue.h>
#include <spdlog/spdlog.h>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

struct StashAPIReply {
  StashAPIReply() { timer.start(); }

  bool          fetchingNextChangeId = false;
  QByteArray    data;
  QElapsedTimer timer;
};

/**
 * In charge of fetching the latest data from the public stash api.
 */
class FetcherPSAPI : public QObject {
  Q_OBJECT
 public:
  explicit FetcherPSAPI(QObject* parent = nullptr);

  void init(spdlog::sinks_init_list                                    sinks,
            const QString&                                             next_change_id,
            std::shared_ptr<moodycamel::ReaderWriterQueue<QByteArray>> rawDataQueue);

 signals:
  void fetched();

 private:
  bool canFetch();
  void fetch(const QString& next_change_id);

  void handleNewData(QNetworkReply* reply, bool stopOnRateLimiting = false);

  void handleFetched(QNetworkReply* reply);

  QString extractNextChangeId(const QByteArray& data);
  bool    checkRateLimits(QNetworkReply* reply);
  void    waitRateLimitingTimeout(QNetworkReply* reply);

 private:
  QNetworkAccessManager*               mAccessManager;
  std::shared_ptr<spdlog::logger>      mLogger;
  QHash<QNetworkReply*, StashAPIReply> mReplies;
  QString                              mLastChangeId;

  std::shared_ptr<moodycamel::ReaderWriterQueue<QByteArray>> mRawDataQueue;

  QElapsedTimer mTotalTimer;
  double        mTotalSize = 0;

  std::vector<std::chrono::high_resolution_clock::time_point> mFetchTime;
};
