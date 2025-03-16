#include "DatabaseManager.h"

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

DatabaseManager::DatabaseManager(const string &conninfo)
    : conn(make_unique<connection>(conninfo)) {
  if (!conn->is_open())
    throw runtime_error("Failed to open database connection");
}

connection *DatabaseManager::getConnection() { return conn.get(); }

void DatabaseManager::setupTables() {
  vector<unique_ptr<DBTable>> dropTables;
  dropTables.push_back(make_unique<PlayerTable>());
  dropTables.push_back(make_unique<TeamTable>());
  dropTables.push_back(make_unique<StateTable>());
  dropTables.push_back(make_unique<ColorTable>());
  for (const auto &table : dropTables) table->drop(conn.get());

  vector<unique_ptr<DBTable>> createTables;
  createTables.push_back(make_unique<StateTable>());
  createTables.push_back(make_unique<ColorTable>());
  createTables.push_back(make_unique<TeamTable>());
  createTables.push_back(make_unique<PlayerTable>());
  for (const auto &table : createTables) table->create(conn.get());
}

void DatabaseManager::loadAndInsertRecords() {
  vector<unique_ptr<DBRecord>> allRecords;

  auto createStateRecord = [](istringstream &iss) -> unique_ptr<DBRecord> {
    int skip;
    string name;
    if (!(iss >> skip >> name)) {
      cerr << "Error parsing state line." << endl;
      return nullptr;
    }
    return make_unique<StateRecord>(name);
  };

  auto createColorRecord = [](istringstream &iss) -> unique_ptr<DBRecord> {
    int skip;
    string name;
    if (!(iss >> skip >> name)) {
      cerr << "Error parsing color line." << endl;
      return nullptr;
    }
    return make_unique<ColorRecord>(name);
  };

  auto createTeamRecord = [](istringstream &iss) -> unique_ptr<DBRecord> {
    int skip, sid, cid, wins, losses;
    string name;
    if (!(iss >> skip >> name >> sid >> cid >> wins >> losses)) {
      cerr << "Error parsing team line." << endl;
      return nullptr;
    }
    return make_unique<TeamRecord>(name, sid, cid, wins, losses);
  };

  auto createPlayerRecord = [](istringstream &iss) -> unique_ptr<DBRecord> {
    int skip, tid, unum, mpg, ppg, rpg, apg;
    double spg, bpg;
    string fn, ln;
    if (!(iss >> skip >> tid >> unum >> fn >> ln >> mpg >> ppg >> rpg >> apg >>
          spg >> bpg)) {
      cerr << "Error parsing player line." << endl;
      return nullptr;
    }
    return make_unique<PlayerRecord>(tid, unum, fn, ln, mpg, ppg, rpg, apg, spg,
                                     bpg);
  };

  appendRecords(allRecords, parseFile("state.txt", createStateRecord));
  appendRecords(allRecords, parseFile("color.txt", createColorRecord));
  appendRecords(allRecords, parseFile("team.txt", createTeamRecord));
  appendRecords(allRecords, parseFile("player.txt", createPlayerRecord));

  for (const auto &record : allRecords) record->insertIntoDB(conn.get());
}

void DatabaseManager::runTests() { exercise(conn.get()); }

template <typename Creator>
vector<unique_ptr<DBRecord>> DatabaseManager::parseFile(const string &filename,
                                                        Creator createRecord) {
  vector<unique_ptr<DBRecord>> records;
  ifstream infile(filename);
  if (!infile.is_open()) {
    cerr << "Error: cannot open " << filename << endl;
    return records;
  }
  string line;
  while (getline(infile, line)) {
    if (line.empty()) continue;
    istringstream iss(line);
    unique_ptr<DBRecord> rec = createRecord(iss);
    if (rec) records.push_back(move(rec));
  }
  return records;
}

void DatabaseManager::appendRecords(vector<unique_ptr<DBRecord>> &dest,
                                    vector<unique_ptr<DBRecord>> src) {
  for (auto &rec : src) dest.push_back(move(rec));
}
