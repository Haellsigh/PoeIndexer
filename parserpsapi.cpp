#include <simdjson.h>
#include <spdlog/spdlog.h>

#include "parserpsapi.hh"

ParserPSAPI::ParserPSAPI(spdlog::sinks_init_list sinks) : logger_(std::make_shared<spdlog::logger>("parser", sinks)) {
    logger_->set_level(spdlog::level::trace);

    logger_->info("Initialized");
}

void ParserPSAPI::init() {}

void ParserPSAPI::parse() {
    using namespace simdjson;

    // std::vector<uint8_t> data;
    /*while (mRawDataQueue->try_dequeue(data)) {
        // QElapsedTimer timer;
        // timer.start();

        dom::parser parser;
        dom::object object;
        error_code error;*/

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
    //}

    // emit parsed();
}
