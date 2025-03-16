#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <sstream>
#include <string>
#include <vector>

#include "exerciser.h"
#include "query_funcs.h"  // so we can call add_player, add_team, etc.

using namespace std;
using namespace pqxx;

// We keep these structs/classes only to help parse each line, but
// we will NOT store the first ID column (since we have no place to put it).
class Player {
 public:
  // We do not need player_id because we cannot use it anyway
  int team_id;
  int uniform_num;
  std::string first_name;
  std::string last_name;
  int mpg;
  int ppg;
  int rpg;
  int apg;
  double spg;
  double bpg;

  Player(int tid, int unum, const std::string &fname, const std::string &lname,
         int m, int p, int r, int a, double s, double b)
      : team_id(tid),
        uniform_num(unum),
        first_name(fname),
        last_name(lname),
        mpg(m),
        ppg(p),
        rpg(r),
        apg(a),
        spg(s),
        bpg(b) {}
};

std::vector<Player> parse_player_file(const std::string &filename) {
  std::vector<Player> players;
  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    if (line.empty()) continue;
    std::istringstream iss(line);

    // The original file format is:
    // <PLAYER_ID> <TEAM_ID> <UNIFORM_NUM> <FIRST_NAME> <LAST_NAME> <MPG> <PPG>
    // <RPG> <APG> <SPG> <BPG> but we cannot use PLAYER_ID with the existing
    // function signature.
    int skip_player_id;
    int team_id, uniform_num, mpg, ppg, rpg, apg;
    double spg, bpg;
    std::string first_name, last_name;

    if (!(iss >> skip_player_id >> team_id >> uniform_num >> first_name >>
          last_name >> mpg >> ppg >> rpg >> apg >> spg >> bpg)) {
      std::cerr << "Error parsing line: " << line << std::endl;
      continue;
    }
    players.push_back(Player(team_id, uniform_num, first_name, last_name, mpg,
                             ppg, rpg, apg, spg, bpg));
  }
  infile.close();
  return players;
}

class Team {
 public:
  // No team_id field because we can't pass it to add_team anyway
  std::string name;
  int state_id;
  int color_id;
  int wins;
  int losses;

  Team(const std::string &n, int s_id, int c_id, int w, int l)
      : name(n), state_id(s_id), color_id(c_id), wins(w), losses(l) {}
};

std::vector<Team> parse_team_file(const std::string &filename) {
  std::vector<Team> teams;
  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    if (line.empty()) continue;
    std::istringstream iss(line);

    // The file format is:
    // <TEAM_ID> <NAME> <STATE_ID> <COLOR_ID> <WINS> <LOSSES>
    // but the function add_team(...) does not accept TEAM_ID
    int skip_team_id;
    std::string n;
    int s_id, c_id, w, l;
    if (!(iss >> skip_team_id >> n >> s_id >> c_id >> w >> l)) {
      std::cerr << "Error parsing line: " << line << std::endl;
      continue;
    }
    teams.push_back(Team(n, s_id, c_id, w, l));
  }
  infile.close();
  return teams;
}

class State {
 public:
  // We ignore the state_id from file
  std::string name;
  State(const std::string &n) : name(n) {}
};

std::vector<State> parse_state_file(const std::string &filename) {
  std::vector<State> states;
  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    if (line.empty()) continue;
    std::istringstream iss(line);

    // Format: <STATE_ID> <NAME>
    int skip_state_id;
    std::string n;
    if (!(iss >> skip_state_id >> n)) {
      std::cerr << "Error parsing line: " << line << std::endl;
      continue;
    }
    states.push_back(State(n));
  }
  infile.close();
  return states;
}

class Color {
 public:
  // We ignore the color_id from file
  std::string name;
  Color(const std::string &n) : name(n) {}
};

std::vector<Color> parse_color_file(const std::string &filename) {
  std::vector<Color> colors;
  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    if (line.empty()) continue;
    std::istringstream iss(line);

    // Format: <COLOR_ID> <NAME>
    int skip_color_id;
    std::string n;
    if (!(iss >> skip_color_id >> n)) {
      std::cerr << "Error parsing line: " << line << std::endl;
      continue;
    }
    colors.push_back(Color(n));
  }
  infile.close();
  return colors;
}

void drop_relation(connection *C) {
  work db(*C);
  // Must drop PLAYER first if we had foreign key constraints.
  // The skeleton does not define real FOREIGN KEY constraints, but
  // we can still do it in this order:
  db.exec("DROP TABLE IF EXISTS PLAYER;");
  db.exec("DROP TABLE IF EXISTS TEAM;");
  db.exec("DROP TABLE IF EXISTS STATE;");
  db.exec("DROP TABLE IF EXISTS COLOR;");
  db.commit();
}

void create_relation(connection *C) {
  work db(*C);

  // Using SERIAL for each ID, so the DB auto-generates the primary key
  db.exec(
      "CREATE TABLE STATE ("
      "STATE_ID SERIAL PRIMARY KEY, "
      "NAME VARCHAR(50)"
      ");");

  db.exec(
      "CREATE TABLE COLOR ("
      "COLOR_ID SERIAL PRIMARY KEY, "
      "NAME VARCHAR(50)"
      ");");

  db.exec(
      "CREATE TABLE TEAM ("
      "TEAM_ID SERIAL PRIMARY KEY, "
      "NAME VARCHAR(50), "
      "STATE_ID INTEGER, "
      "COLOR_ID INTEGER, "
      "WINS INTEGER, "
      "LOSSES INTEGER"
      ");");

  db.exec(
      "CREATE TABLE PLAYER ("
      "PLAYER_ID SERIAL PRIMARY KEY, "
      "TEAM_ID INTEGER, "
      "UNIFORM_NUM INTEGER, "
      "FIRST_NAME VARCHAR(50), "
      "LAST_NAME VARCHAR(50), "
      "MPG INTEGER, "
      "PPG INTEGER, "
      "RPG INTEGER, "
      "APG INTEGER, "
      "SPG DECIMAL(3,1), "
      "BPG DECIMAL(3,1)"
      ");");

  db.commit();
}

void load_states(connection *C, const vector<State> &states) {
  for (auto &st : states) {
    // add_state(C, string name)
    // ignoring any ID from the file, so just pass st.name
    add_state(C, st.name);
  }
}

void load_colors(connection *C, const vector<Color> &colors) {
  for (auto &clr : colors) {
    // add_color(C, string name)
    add_color(C, clr.name);
  }
}

void load_teams(connection *C, const vector<Team> &teams) {
  for (auto &tm : teams) {
    // add_team(C, string name, int state_id, int color_id, int wins, int
    // losses)
    add_team(C, tm.name, tm.state_id, tm.color_id, tm.wins, tm.losses);
  }
}

void load_players(connection *C, const vector<Player> &players) {
  for (auto &pl : players) {
    // add_player(C, int team_id, int jersey_num, string fname, string lname,
    //            int mpg, int ppg, int rpg, int apg, double spg, double bpg)
    add_player(C, pl.team_id, pl.uniform_num, pl.first_name, pl.last_name,
               pl.mpg, pl.ppg, pl.rpg, pl.apg, pl.spg, pl.bpg);
  }
}

int main(int argc, char *argv[]) {
  connection *C = nullptr;
  try {
    // Connect to the ACC_BBALL database as user 'postgres'
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (!C->is_open()) {
      cerr << "Can't open database ACC_BBALL" << endl;
      return 1;
    }
    cout << "Opened database successfully: " << C->dbname() << endl;
  } catch (const exception &e) {
    cerr << e.what() << endl;
    return 1;
  }

  // Drop any existing tables, then recreate them
  drop_relation(C);
  create_relation(C);

  // Parse the text files
  vector<Player> players = parse_player_file("player.txt");
  vector<Team> teams = parse_team_file("team.txt");
  vector<State> states = parse_state_file("state.txt");
  vector<Color> colors = parse_color_file("color.txt");

  // Load data into the DB
  load_states(C, states);
  load_colors(C, colors);
  load_teams(C, teams);
  load_players(C, players);

  // Finally, run your queries/tests
  exercise(C);

  C->disconnect();
  delete C;
  return 0;
}
