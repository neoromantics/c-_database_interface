#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <fstream>
#include <memory>
#include <pqxx/pqxx>

#include "ColorRecord.h"
#include "ColorTable.h"
#include "PlayerRecord.h"
#include "PlayerTable.h"
#include "StateRecord.h"
#include "StateTable.h"
#include "TeamRecord.h"
#include "TeamTable.h"
#include "exerciser.h"

using namespace std;
using namespace pqxx;

class DatabaseManager {
 private:
  unique_ptr<connection> conn;

  // Templated file parsing method.
  template <typename Creator>
  vector<unique_ptr<DBRecord>> parseFile(const string &filename,
                                         Creator createRecord);

  // Helper to append records from one vector into dest.
  void appendRecords(vector<unique_ptr<DBRecord>> &dest,
                     vector<unique_ptr<DBRecord>> src);

 public:
  DatabaseManager(const string &conninfo);

  connection *getConnection();

  // Drop and create tables.
  void setupTables();

  // Load all records from files and insert them.
  void loadAndInsertRecords();

  // Run tests from exerciser.cpp.
  void runTests();
};

#endif  // DATABASEMANAGER_H
