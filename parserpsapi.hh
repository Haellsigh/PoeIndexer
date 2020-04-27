#pragma once

#include <QObject>

#include <readerwriterqueue.h>
#include <spdlog/spdlog.h>

/**
 * In charge of parsing the data received from the FetcherPSAPI.
 */
class ParserPSAPI : public QObject {
  Q_OBJECT
 public:
  explicit ParserPSAPI(QObject* parent = nullptr);

  void init(spdlog::sinks_init_list                                    sinks,
            std::shared_ptr<moodycamel::ReaderWriterQueue<QByteArray>> rawDataQueue);

 signals:
  void parsed();

 public slots:
  void parse();

 private:
  std::shared_ptr<spdlog::logger> mLogger;

  std::shared_ptr<moodycamel::ReaderWriterQueue<QByteArray>> mRawDataQueue;
};
