#include "dbupdater.hh"

#include <spdlog/async.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>

DbUpdater::DbUpdater(spdlog::sinks_init_list sinks) : logger_(std::make_shared<spdlog::logger>("updater", sinks)) {
    logger_->set_level(spdlog::level::trace);
    logger_->info("Initialized");
}

void DbUpdater::init() {

    // Connect to the database
    /*QStringList drivers = QSqlDatabase::drivers();
    if (!drivers.contains("QMYSQL")) {
      mLogger->critical("Couldn't find MySQL driver");
      return;
    }*/

    /*
    auto db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("poe");
    if (!db.open("root", "!Hardstyle5002")) {
      mLogger->critical("Unable to connect to the database: {}",
                        db.lastError().text().toStdString());
    } else {
      mLogger->info("Connected to database {}@{}", db.databaseName().toStdString(),
                    db.hostName().toStdString());
    }*/
}

void DbUpdater::update() {
    logger_->info("Getting account names from db...");

    /*auto db = QSqlDatabase::database();
    if (db.transaction()) {
      QSqlQuery query("SELECT * FROM accounts");
      int       nameField = query.record().indexOf("Name");

      while (query.next()) {
        mLogger->info("Name: {}", query.value(nameField).toString().toStdString());
      }

      if (!db.commit()) {
        mLogger->error("Couldn't commit query: {}", db.lastError().text().toStdString());
      }
    }*/
}
