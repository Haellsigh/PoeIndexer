#include "dbupdater.hh"

#include <QtSql>

DbUpdater::DbUpdater(QObject* parent) : QObject(parent) {}

void DbUpdater::init(spdlog::sinks_init_list                                sinks,
                     std::shared_ptr<moodycamel::ReaderWriterQueue<Stash> > stashQueue) {
  mLogger = std::make_shared<spdlog::logger>("updater", sinks);
  mLogger->set_level(spdlog::level::trace);

  mStashQueue = stashQueue;

  // Connect to the database
  QStringList drivers = QSqlDatabase::drivers();
  if (!drivers.contains("QMYSQL")) {
    mLogger->critical("Couldn't find MySQL driver");
    return;
  }

  auto db = QSqlDatabase::addDatabase("QMYSQL");
  db.setHostName("localhost");
  db.setDatabaseName("poe");
  if (!db.open("root", "!Hardstyle5002")) {
    mLogger->critical("Unable to connect to the database: {}",
                      db.lastError().text().toStdString());
  } else {
    mLogger->info("Connected to database {}@{}", db.databaseName().toStdString(),
                  db.hostName().toStdString());
  }

  mLogger->info("Initialized");
}

void DbUpdater::update() {
  mLogger->info("Getting account names from db...");

  auto db = QSqlDatabase::database();
  if (db.transaction()) {
    QSqlQuery query("SELECT * FROM accounts");
    int       nameField = query.record().indexOf("Name");

    while (query.next()) {
      mLogger->info("Name: {}", query.value(nameField).toString().toStdString());
    }

    if (!db.commit()) {
      mLogger->error("Couldn't commit query: {}", db.lastError().text().toStdString());
    }
  }

  /*
    Method 1
      QSqlQuery query;
      query.prepare("INSERT INTO person (id, forename, surname) "
                    "VALUES (:id, :forename, :surname)");
      query.bindValue(":id", 1001);
      query.bindValue(":forename", "Bart");
      query.bindValue(":surname", "Simpson");
      query.exec();

    Method 2
      QSqlQuery query;
      query.prepare("INSERT INTO person (id, forename, surname) "
          "VALUES (:id, :forename, :surname)");
      query.bindValue(0, 1001);
      query.bindValue(1, "Bart");
      query.bindValue(2, "Simpson");
      query.exec();

    Method 3
      QSqlQuery query;
      query.prepare("INSERT INTO person (id, forename, surname) "
                    "VALUES (?, ?, ?)");
      query.bindValue(0, 1001);
      query.bindValue(1, "Bart");
      query.bindValue(2, "Simpson");
      query.exec();

    Method 4
      QSqlQuery query;
      query.prepare("INSERT INTO person (id, forename, surname) "
                    "VALUES (?, ?, ?)");
      query.addBindValue(1001);
      query.addBindValue("Bart");
      query.addBindValue("Simpson");
      query.exec();
  */
}