#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <sstream>
#include <string>
#include <vector>

#include "exerciser.h"
#include "query_funcs.h"

using namespace std;
using namespace pqxx;

// Since the add_* function signatures don't accept the "ID" from the file,
// we only store the rest of the fields. The ID in each table will be
// assigned automatically by SERIAL.

class Player {
 public:
  // ignoring the first column (PLAYER_ID) from file
  int team_id;
  int uniform_num;
  string first_name;
  string last_name;
  int mpg;
  int ppg;
  int rpg;
  int apg;
  double spg;
  double bpg;

  Player(int tid, int unum, const string &fn, const string &ln, int m, int p,
         int r, int a, double s, double b)
      : team_id(tid),
        uniform_num(unum),
        first_name(fn),
        last_name(ln),
        mpg(m),
        ppg(p),
        rpg(r),
        apg(a),
        spg(s),
        bpg(b) {}
};

static vector<Player> parse_player_file(const string &filename) {
  vector<Player> players;
  ifstream infile(filename);
  string line;
  while (getline(infile, line)) {
    if (line.empty()) continue;
    istringstream iss(line);
    // file format: <PLAYER_ID> <TEAM_ID> <UNIFORM_NUM> <FIRST_NAME> <LAST_NAME>
    // <MPG> <PPG> <RPG> <APG> <SPG> <BPG>
    int skip_player_id;
    int tid, unum, mpg, ppg, rpg, apg;
    double spg, bpg;
    string fn, ln;
    if (!(iss >> skip_player_id >> tid >> unum >> fn >> ln >> mpg >> ppg >>
          rpg >> apg >> spg >> bpg)) {
      cerr << "Error parsing line (player): " << line << endl;
      continue;
    }
    players.push_back(Player(tid, unum, fn, ln, mpg, ppg, rpg, apg, spg, bpg));
  }
  infile.close();
  return players;
}

class Team {
 public:
  // ignoring <TEAM_ID>
  string name;
  int state_id;
  int color_id;
  int wins;
  int losses;

  Team(const string &n, int sid, int cid, int w, int l)
      : name(n), state_id(sid), color_id(cid), wins(w), losses(l) {}
};

static vector<Team> parse_team_file(const string &filename) {
  vector<Team> teams;
  ifstream infile(filename);
  string line;
  while (getline(infile, line)) {
    if (line.empty()) continue;
    istringstream iss(line);
    // format: <TEAM_ID> <NAME> <STATE_ID> <COLOR_ID> <WINS> <LOSSES>
    int skip_team_id;
    string n;
    int sid, cid, w, l;
    if (!(iss >> skip_team_id >> n >> sid >> cid >> w >> l)) {
      cerr << "Error parsing line (team): " << line << endl;
      continue;
    }
    teams.push_back(Team(n, sid, cid, w, l));
  }
  infile.close();
  return teams;
}

class State {
 public:
  // ignoring <STATE_ID>
  string name;
  State(const string &n) : name(n) {}
};

static vector<State> parse_state_file(const string &filename) {
  vector<State> states;
  ifstream infile(filename);
  string line;
  while (getline(infile, line)) {
    if (line.empty()) continue;
    istringstream iss(line);
    // format: <STATE_ID> <NAME>
    int skip_state_id;
    string n;
    if (!(iss >> skip_state_id >> n)) {
      cerr << "Error parsing line (state): " << line << endl;
      continue;
    }
    states.push_back(State(n));
  }
  infile.close();
  return states;
}

class Color {
 public:
  // ignoring <COLOR_ID>
  string name;
  Color(const string &n) : name(n) {}
};

static vector<Color> parse_color_file(const string &filename) {
  vector<Color> colors;
  ifstream infile(filename);
  string line;
  while (getline(infile, line)) {
    if (line.empty()) continue;
    istringstream iss(line);
    // format: <COLOR_ID> <NAME>
    int skip_color_id;
    string n;
    if (!(iss >> skip_color_id >> n)) {
      cerr << "Error parsing line (color): " << line << endl;
      continue;
    }
    colors.push_back(Color(n));
  }
  infile.close();
  return colors;
}

static void drop_relation(connection *C) {
  work W(*C);
  W.exec("DROP TABLE IF EXISTS PLAYER;");
  W.exec("DROP TABLE IF EXISTS TEAM;");
  W.exec("DROP TABLE IF EXISTS STATE;");
  W.exec("DROP TABLE IF EXISTS COLOR;");
  W.commit();
}

static void create_relation(connection *C) {
  work W(*C);

  // We use SERIAL for the ID columns, matching the original skeleton approach.
  W.exec(
      "CREATE TABLE STATE("
      "STATE_ID SERIAL PRIMARY KEY, "
      "NAME VARCHAR(50));");

  W.exec(
      "CREATE TABLE COLOR("
      "COLOR_ID SERIAL PRIMARY KEY, "
      "NAME VARCHAR(50));");

  W.exec(
      "CREATE TABLE TEAM("
      "TEAM_ID SERIAL PRIMARY KEY, "
      "NAME VARCHAR(50), "
      "STATE_ID INT, "
      "COLOR_ID INT, "
      "WINS INT, "
      "LOSSES INT);");

  W.exec(
      "CREATE TABLE PLAYER("
      "PLAYER_ID SERIAL PRIMARY KEY, "
      "TEAM_ID INT, "
      "UNIFORM_NUM INT, "
      "FIRST_NAME VARCHAR(50), "
      "LAST_NAME VARCHAR(50), "
      "MPG INT, "
      "PPG INT, "
      "RPG INT, "
      "APG INT, "
      "SPG DECIMAL(3,1), "
      "BPG DECIMAL(3,1));");

  W.commit();
}

static void load_states(connection *C, const vector<State> &states) {
  for (auto &st : states) {
    // add_state(C, string name)
    add_state(C, st.name);
  }
}

static void load_colors(connection *C, const vector<Color> &colors) {
  for (auto &c : colors) {
    // add_color(C, string name)
    add_color(C, c.name);
  }
}

static void load_teams(connection *C, const vector<Team> &teams) {
  for (auto &t : teams) {
    // add_team(C, string name, int state_id, int color_id, int wins, int
    // losses)
    add_team(C, t.name, t.state_id, t.color_id, t.wins, t.losses);
  }
}

static void load_players(connection *C, const vector<Player> &players) {
  for (auto &p : players) {
    // add_player(C, int team_id, int jersey_num, string fname, string lname,
    //            int mpg, int ppg, int rpg, int apg, double spg, double bpg)
    add_player(C, p.team_id, p.uniform_num, p.first_name, p.last_name, p.mpg,
               p.ppg, p.rpg, p.apg, p.spg, p.bpg);
  }
}

int main(int argc, char *argv[]) {
  connection *C = nullptr;
  try {
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (!C->is_open()) {
      cerr << "Cannot open database ACC_BBALL" << endl;
      return 1;
    }
    cout << "Opened database successfully: " << C->dbname() << endl;
  } catch (const exception &e) {
    cerr << e.what() << endl;
    return 1;
  }

  // Drop & create tables
  drop_relation(C);
  create_relation(C);

  // Parse data files
  vector<Player> players = parse_player_file("player.txt");
  vector<Team> teams = parse_team_file("team.txt");
  vector<State> states = parse_state_file("state.txt");
  vector<Color> colors = parse_color_file("color.txt");

  // Load data
  load_states(C, states);
  load_colors(C, colors);
  load_teams(C, teams);
  load_players(C, players);

  // Now run the test queries in exerciser.cpp
  exercise(C);

  C->disconnect();
  delete C;
  return 0;
}
