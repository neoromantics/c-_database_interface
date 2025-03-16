#include "query_funcs.h"

#include <iomanip>
#include <iostream>
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
    ss << "INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, "
          "LAST_NAME, "
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
    ss << "INSERT INTO TEAM ( NAME, STATE_ID, COLOR_ID, WINS, LOSSES) "
          "VALUES ("
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
    ss << "INSERT INTO COLOR ( NAME) VALUES (" << W.quote(name) << ");";
    W.exec(ss.str());
    W.commit();
  } catch (const exception &e) {
    cerr << "Error in add_color: " << e.what() << endl;
  }
}

result run_query(connection *C, const std::string &sql) {
  work W(*C);
  result R = W.exec(sql);
  W.commit();
  return R;
}

static void print_query1_row(const row &r) {
  cout << r[0].as<int>() << " " << r[1].as<int>() << " " << r[2].as<int>()
       << " " << r[3].as<string>() << " " << r[4].as<string>() << " "
       << r[5].as<int>() << " " << r[6].as<int>() << " " << r[7].as<int>()
       << " " << r[8].as<int>() << " " << fixed << setprecision(1)
       << r[9].as<double>() << " " << fixed << setprecision(1)
       << r[10].as<double>() << endl;
}

static void print_query2_row(const row &r) {
  cout << r[0].as<string>() << endl;
}

static void print_query3_row(const row &r) {
  cout << r[0].as<string>() << " " << r[1].as<string>() << endl;
}

static void print_query4_row(const row &r) {
  cout << r[0].as<int>() << " " << r[1].as<string>() << " " << r[2].as<string>()
       << endl;
}

static void print_query5_row(const row &r) {
  cout << r[0].as<string>() << " " << r[1].as<string>() << " "
       << r[2].as<string>() << " " << r[3].as<int>() << endl;
}

void query1(connection *C, int use_mpg, int min_mpg, int max_mpg, int use_ppg,
            int min_ppg, int max_ppg, int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg, int use_spg, double min_spg,
            double max_spg, int use_bpg, double min_bpg, double max_bpg) {
  try {
    std::stringstream ss;
    ss << "SELECT PLAYER_ID, TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, "
       << "MPG, PPG, RPG, APG, SPG, BPG FROM PLAYER";

    vector<string> conditions;
    if (use_mpg) {
      conditions.push_back("MPG BETWEEN " + to_string(min_mpg) + " AND " +
                           to_string(max_mpg));
    }
    if (use_ppg) {
      conditions.push_back("PPG BETWEEN " + to_string(min_ppg) + " AND " +
                           to_string(max_ppg));
    }
    if (use_rpg) {
      conditions.push_back("RPG BETWEEN " + to_string(min_rpg) + " AND " +
                           to_string(max_rpg));
    }
    if (use_apg) {
      conditions.push_back("APG BETWEEN " + to_string(min_apg) + " AND " +
                           to_string(max_apg));
    }
    if (use_spg) {
      conditions.push_back("SPG BETWEEN " + to_string(min_spg) + " AND " +
                           to_string(max_spg));
    }
    if (use_bpg) {
      conditions.push_back("BPG BETWEEN " + to_string(min_bpg) + " AND " +
                           to_string(max_bpg));
    }

    if (!conditions.empty()) {
      ss << " WHERE ";
      for (size_t i = 0; i < conditions.size(); i++) {
        if (i > 0) ss << " AND ";
        ss << conditions[i];
      }
    }
    ss << ";";

    result R = run_query(C, ss.str());
    cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG "
            "APG SPG BPG"
         << endl;
    for (const auto &row : R) {
      print_query1_row(row);
    }
  } catch (const exception &e) {
    cerr << "Error in query1: " << e.what() << endl;
  }
}

void query2(connection *C, string team_color) {
  try {
    stringstream ss;
    ss << "SELECT TEAM.NAME "
       << "FROM TEAM JOIN COLOR ON TEAM.COLOR_ID = COLOR.COLOR_ID "
       << "WHERE COLOR.NAME = " << work(*C).quote(team_color) << ";";

    result R = run_query(C, ss.str());
    cout << "NAME" << endl;
    for (const auto &row : R) {
      print_query2_row(row);
    }
  } catch (const exception &e) {
    cerr << "Error in query2: " << e.what() << endl;
  }
}

void query3(connection *C, string team_name) {
  try {
    stringstream ss;
    ss << "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME "
       << "FROM PLAYER JOIN TEAM ON PLAYER.TEAM_ID = TEAM.TEAM_ID "
       << "WHERE TEAM.NAME = " << work(*C).quote(team_name)
       << " ORDER BY PLAYER.PPG DESC;";

    result R = run_query(C, ss.str());
    cout << "FIRST_NAME LAST_NAME" << endl;
    for (const auto &row : R) {
      print_query3_row(row);
    }
  } catch (const exception &e) {
    cerr << "Error in query3: " << e.what() << endl;
  }
}

void query4(connection *C, string team_state, string team_color) {
  try {
    stringstream ss;
    ss << "SELECT PLAYER.UNIFORM_NUM, PLAYER.FIRST_NAME, PLAYER.LAST_NAME "
       << "FROM PLAYER JOIN TEAM ON PLAYER.TEAM_ID = TEAM.TEAM_ID "
       << "JOIN STATE ON TEAM.STATE_ID = STATE.STATE_ID "
       << "JOIN COLOR ON TEAM.COLOR_ID = COLOR.COLOR_ID "
       << "WHERE STATE.NAME = " << work(*C).quote(team_state)
       << " AND COLOR.NAME = " << work(*C).quote(team_color) << ";";

    result R = run_query(C, ss.str());
    cout << "UNIFORM_NUM FIRST_NAME LAST_NAME" << endl;
    for (const auto &row : R) {
      print_query4_row(row);
    }
  } catch (const exception &e) {
    cerr << "Error in query4: " << e.what() << endl;
  }
}

void query5(connection *C, int num_wins) {
  try {
    stringstream ss;
    ss << "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME, TEAM.NAME, TEAM.WINS "
       << "FROM PLAYER JOIN TEAM ON PLAYER.TEAM_ID = TEAM.TEAM_ID "
       << "WHERE TEAM.WINS > " << num_wins << ";";

    result R = run_query(C, ss.str());
    cout << "FIRST_NAME LAST_NAME NAME WINS" << endl;
    for (const auto &row : R) {
      print_query5_row(row);
    }
  } catch (const exception &e) {
    cerr << "Error in query5: " << e.what() << endl;
  }
}
