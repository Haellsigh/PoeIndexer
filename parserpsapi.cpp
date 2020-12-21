#include "parserpsapi.hh"

#include <simdjson/simdjson.h>

ParserPSAPI::ParserPSAPI(/*QObject* parent*/) /*: QObject(parent)*/ {}

void ParserPSAPI::init(
    spdlog::sinks_init_list                                              sinks,
    std::shared_ptr<moodycamel::ReaderWriterQueue<std::vector<uint8_t>>> rawDataQueue) {
  mLogger = std::make_shared<spdlog::logger>("parser", sinks);
  mLogger->set_level(spdlog::level::trace);

  mRawDataQueue = rawDataQueue;

  mLogger->info("Initialized");
}

void ParserPSAPI::parse() {
  using namespace simdjson;

  std::vector<uint8_t> data;
  while (mRawDataQueue->try_dequeue(data)) {
    // QElapsedTimer timer;
    // timer.start();

    dom::parser parser;
    dom::object object;
    error_code  error;

    /*parser.parse(padded_string(data.data(), data.size()))
        .get<dom::object>()
        .tie(object, error);

    if (error || object.size() != 2) {
      mLogger->error("Invalid data received");
      return;
    }

    int nStashes = 0, nStashesDelirium = 0, nItems = 0, nItemsDelirium = 0;

    dom::element stashes;
    object["stashes"].tie(stashes, error);

    if (error || !stashes.is<dom::array>()) {
      mLogger->error("Could not find stashes data in json");
      return;
    }

    for (const auto& stash : stashes) {
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
        mLogger->error("Json error parsing items: {}", error);
        continue;
      }

      nItems += items.size();

      if (league == std::string_view("Delirium")) {
        nStashesDelirium++;
        nItemsDelirium += items.size();
        // mLogger->trace("Account {}, stash id {}", stash["accountName"], stash["id"]);
      } else {
      }
    }*/

    /*mLogger->debug("Parsed {}/{} items within {}/{} stashes in {} ms", nItemsDelirium,
                   nItems, nStashesDelirium, nStashes, timer.elapsed());*/
  }

  // emit parsed();
}
