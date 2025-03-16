#include "query_funcs.h"

#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <pqxx/pqxx>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace pqxx;

void add_player(connection *C, int team_id, int jersey_num, string first_name,
                string last_name, int mpg, int ppg, int rpg, int apg,
                double spg, double bpg) {
  try {
    work W(*C);
    stringstream ss;
    ss << "INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, "
          "MPG, PPG, RPG, APG, SPG, BPG) VALUES ("
       << team_id << ", " << jersey_num << ", " << W.quote(first_name) << ", "
       << W.quote(last_name) << ", " << mpg << ", " << ppg << ", " << rpg
       << ", " << apg << ", " << spg << ", " << bpg << ");";
    W.exec(ss.str());
    W.commit();
  } catch (const exception &e) {
    cerr << "Error in add_player: " << e.what() << endl;
  }
}

void add_team(connection *C, string name, int state_id, int color_id, int wins,
              int losses) {
  try {
    work W(*C);
    stringstream ss;
    ss << "INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES ("
       << W.quote(name) << ", " << state_id << ", " << color_id << ", " << wins
       << ", " << losses << ");";
    W.exec(ss.str());
    W.commit();
  } catch (const exception &e) {
    cerr << "Error in add_team: " << e.what() << endl;
  }
}

void add_state(connection *C, string name) {
  try {
    work W(*C);
    stringstream ss;
    ss << "INSERT INTO STATE (NAME) VALUES (" << W.quote(name) << ");";
    W.exec(ss.str());
    W.commit();
  } catch (const exception &e) {
    cerr << "Error in add_state: " << e.what() << endl;
  }
}

void add_color(connection *C, string name) {
  try {
    work W(*C);
    stringstream ss;
    ss << "INSERT INTO COLOR (NAME) VALUES (" << W.quote(name) << ");";
    W.exec(ss.str());
    W.commit();
  } catch (const exception &e) {
    cerr << "Error in add_color: " << e.what() << endl;
  }
}

static result run_select_query(connection *C, const string &sql) {
  nontransaction N(*C);
  result R = N.exec(sql);
  return R;
}

class Query {
 public:
  virtual ~Query() {}
  virtual void execute(connection *C) const = 0;
};

template <typename RowPrinter>
class SimpleQuery : public Query {
 private:
  string sql;
  string header;
  RowPrinter printRow;

 public:
  SimpleQuery(const string &sql, const string &header, RowPrinter printRow)
      : sql(sql), header(header), printRow(printRow) {}

  void execute(connection *C) const override {
    try {
      result R = run_select_query(C, sql);
      cout << header << endl;
      for (const auto &row : R) printRow(row);
    } catch (const exception &e) {
      cerr << "Error executing query: " << e.what() << endl;
    }
  }
};

class Query1 : public Query {
 private:
  int use_mpg, min_mpg, max_mpg;
  int use_ppg, min_ppg, max_ppg;
  int use_rpg, min_rpg, max_rpg;
  int use_apg, min_apg, max_apg;
  int use_spg;
  double min_spg, max_spg;
  int use_bpg;
  double min_bpg, max_bpg;

 public:
  Query1(int use_mpg, int min_mpg, int max_mpg, int use_ppg, int min_ppg,
         int max_ppg, int use_rpg, int min_rpg, int max_rpg, int use_apg,
         int min_apg, int max_apg, int use_spg, double min_spg, double max_spg,
         int use_bpg, double min_bpg, double max_bpg)
      : use_mpg(use_mpg),
        min_mpg(min_mpg),
        max_mpg(max_mpg),
        use_ppg(use_ppg),
        min_ppg(min_ppg),
        max_ppg(max_ppg),
        use_rpg(use_rpg),
        min_rpg(min_rpg),
        max_rpg(max_rpg),
        use_apg(use_apg),
        min_apg(min_apg),
        max_apg(max_apg),
        use_spg(use_spg),
        min_spg(min_spg),
        max_spg(max_spg),
        use_bpg(use_bpg),
        min_bpg(min_bpg),
        max_bpg(max_bpg) {}

  void execute(connection *C) const override {
    stringstream ss;
    ss << "SELECT PLAYER_ID, TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, "
          "MPG, PPG, RPG, APG, SPG, BPG FROM PLAYER";
    vector<string> conditions;
    if (use_mpg)
      conditions.push_back("MPG BETWEEN " + to_string(min_mpg) + " AND " +
                           to_string(max_mpg));
    if (use_ppg)
      conditions.push_back("PPG BETWEEN " + to_string(min_ppg) + " AND " +
                           to_string(max_ppg));
    if (use_rpg)
      conditions.push_back("RPG BETWEEN " + to_string(min_rpg) + " AND " +
                           to_string(max_rpg));
    if (use_apg)
      conditions.push_back("APG BETWEEN " + to_string(min_apg) + " AND " +
                           to_string(max_apg));
    if (use_spg)
      conditions.push_back("SPG BETWEEN " + to_string(min_spg) + " AND " +
                           to_string(max_spg));
    if (use_bpg)
      conditions.push_back("BPG BETWEEN " + to_string(min_bpg) + " AND " +
                           to_string(max_bpg));
    if (!conditions.empty()) {
      ss << " WHERE ";
      for (size_t i = 0; i < conditions.size(); i++) {
        if (i > 0) ss << " AND ";
        ss << conditions[i];
      }
    }
    ss << ";";

    std::function<void(const row &)> printRow = [](const row &r) {
      cout << r[0].as<int>() << " " << r[1].as<int>() << " " << r[2].as<int>()
           << " " << r[3].as<string>() << " " << r[4].as<string>() << " "
           << r[5].as<int>() << " " << r[6].as<int>() << " " << r[7].as<int>()
           << " " << r[8].as<int>() << " " << fixed << setprecision(1)
           << r[9].as<double>() << " " << fixed << setprecision(1)
           << r[10].as<double>() << endl;
    };
    SimpleQuery<std::function<void(const row &)>> q(
        ss.str(),
        "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG "
        "SPG BPG",
        printRow);
    q.execute(C);
  }
};

auto printQuery2Row = [](const row &r) { cout << r[0].as<string>() << endl; };

auto printQuery3Row = [](const row &r) {
  cout << r[0].as<string>() << " " << r[1].as<string>() << endl;
};

auto printQuery4Row = [](const row &r) {
  cout << r[0].as<int>() << " " << r[1].as<string>() << " " << r[2].as<string>()
       << endl;
};

auto printQuery5Row = [](const row &r) {
  cout << r[0].as<string>() << " " << r[1].as<string>() << " "
       << r[2].as<string>() << " " << r[3].as<int>() << endl;
};

void query1(connection *C, int use_mpg, int min_mpg, int max_mpg, int use_ppg,
            int min_ppg, int max_ppg, int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg, int use_spg, double min_spg,
            double max_spg, int use_bpg, double min_bpg, double max_bpg) {
  Query1 q(use_mpg, min_mpg, max_mpg, use_ppg, min_ppg, max_ppg, use_rpg,
           min_rpg, max_rpg, use_apg, min_apg, max_apg, use_spg, min_spg,
           max_spg, use_bpg, min_bpg, max_bpg);
  q.execute(C);
}

void query2(connection *C, string team_color) {
  string quoted_color = work(*C).quote(team_color);
  stringstream ss;
  ss << "SELECT TEAM.NAME FROM TEAM "
     << "JOIN COLOR ON TEAM.COLOR_ID = COLOR.COLOR_ID "
     << "WHERE COLOR.NAME = " << quoted_color << ";";
  SimpleQuery<decltype(printQuery2Row)> q(ss.str(), "NAME", printQuery2Row);
  q.execute(C);
}

void query3(connection *C, string team_name) {
  string quoted_name = work(*C).quote(team_name);
  stringstream ss;
  ss << "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME "
     << "FROM PLAYER JOIN TEAM ON PLAYER.TEAM_ID = TEAM.TEAM_ID "
     << "WHERE TEAM.NAME = " << quoted_name << " ORDER BY PLAYER.PPG DESC;";
  SimpleQuery<decltype(printQuery3Row)> q(ss.str(), "FIRST_NAME LAST_NAME",
                                          printQuery3Row);
  q.execute(C);
}

void query4(connection *C, string team_state, string team_color) {
  string quoted_state = work(*C).quote(team_state);
  string quoted_color = work(*C).quote(team_color);
  stringstream ss;
  ss << "SELECT PLAYER.UNIFORM_NUM, PLAYER.FIRST_NAME, PLAYER.LAST_NAME "
     << "FROM PLAYER "
     << "JOIN TEAM  ON PLAYER.TEAM_ID = TEAM.TEAM_ID "
     << "JOIN STATE ON TEAM.STATE_ID  = STATE.STATE_ID "
     << "JOIN COLOR ON TEAM.COLOR_ID  = COLOR.COLOR_ID "
     << "WHERE STATE.NAME = " << quoted_state
     << " AND COLOR.NAME = " << quoted_color << ";";
  SimpleQuery<decltype(printQuery4Row)> q(
      ss.str(), "UNIFORM_NUM FIRST_NAME LAST_NAME", printQuery4Row);
  q.execute(C);
}

void query5(connection *C, int num_wins) {
  stringstream ss;
  ss << "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME, TEAM.NAME, TEAM.WINS "
     << "FROM PLAYER JOIN TEAM ON PLAYER.TEAM_ID = TEAM.TEAM_ID "
     << "WHERE TEAM.WINS > " << num_wins << ";";
  SimpleQuery<decltype(printQuery5Row)> q(
      ss.str(), "FIRST_NAME LAST_NAME NAME WINS", printQuery5Row);
  q.execute(C);
}
