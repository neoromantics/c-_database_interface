#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <sstream>
#include <string>
#include <vector>

#include "exerciser.h"

using namespace std;
using namespace pqxx;

class Player {
 public:
  int player_id;
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

  Player(int pid, int tid, int unum, const std::string &fname,
         const std::string &lname, int mpg, int ppg, int rpg, int apg,
         double spg, double bpg)
      : player_id(pid),
        team_id(tid),
        uniform_num(unum),
        first_name(fname),
        last_name(lname),
        mpg(mpg),
        ppg(ppg),
        rpg(rpg),
        apg(apg),
        spg(spg),
        bpg(bpg) {}
};

std::vector<Player> parse_player_file(const std::string &filename) {
  std::vector<Player> players;
  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    if (line.empty()) continue;
    std::istringstream iss(line);
    int player_id, team_id, uniform_num;
    int mpg, ppg, rpg, apg;
    double spg, bpg;
    std::string first_name, last_name;

    if (!(iss >> player_id >> team_id >> uniform_num >> first_name >>
          last_name >> mpg >> ppg >> rpg >> apg >> spg >> bpg)) {
      std::cerr << "Error parsing line: " << line << std::endl;
      continue;
    }
    players.push_back(Player(player_id, team_id, uniform_num, first_name,
                             last_name, mpg, ppg, rpg, apg, spg, bpg));
  }
  infile.close();
  return players;
}

class Team {
 public:
  int team_id;
  std::string name;
  int state_id;
  int color_id;
  int wins;
  int losses;

  Team(int tid, const std::string &name, int state_id, int color_id, int wins,
       int losses)
      : team_id(tid),
        name(name),
        state_id(state_id),
        color_id(color_id),
        wins(wins),
        losses(losses) {}
};

std::vector<Team> parse_team_file(const std::string &filename) {
  std::vector<Team> teams;
  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    if (line.empty()) continue;
    std::istringstream iss(line);
    int team_id, state_id, color_id, wins, losses;
    std::string name;
    if (!(iss >> team_id >> name >> state_id >> color_id >> wins >> losses)) {
      std::cerr << "Error parsing line: " << line << std::endl;
      continue;
    }
    teams.push_back(Team(team_id, name, state_id, color_id, wins, losses));
  }
  infile.close();
  return teams;
}

class State {
 public:
  int state_id;
  std::string name;

  State(int sid, const std::string &name) : state_id(sid), name(name) {}
};

std::vector<State> parse_state_file(const std::string &filename) {
  std::vector<State> states;
  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    if (line.empty()) continue;
    std::istringstream iss(line);
    int state_id;
    std::string name;
    if (!(iss >> state_id >> name)) {
      std::cerr << "Error parsing line: " << line << std::endl;
      continue;
    }
    states.push_back(State(state_id, name));
  }
  infile.close();
  return states;
}

class Color {
 public:
  int color_id;
  std::string name;

  Color(int cid, const std::string &name) : color_id(cid), name(name) {}
};

std::vector<Color> parse_color_file(const std::string &filename) {
  std::vector<Color> colors;
  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    if (line.empty()) continue;
    std::istringstream iss(line);
    int color_id;
    std::string name;
    if (!(iss >> color_id >> name)) {
      std::cerr << "Error parsing line: " << line << std::endl;
      continue;
    }
    colors.push_back(Color(color_id, name));
  }
  infile.close();
  return colors;
}

void drop_relation(connection *C) {
  work db(*C);
  db.exec("DROP TABLE IF EXISTS PLAYER;");
  db.exec("DROP TABLE IF EXISTS TEAM;");
  db.exec("DROP TABLE IF EXISTS STATE;");
  db.exec("DROP TABLE IF EXISTS COLOR;");
  db.commit();
}

void create_relation(connection *C) {
  work db(*C);
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

void load_states(connection *C, vector<State> &states) {
  for (const auto &state : states) {
    add_state(C, state.name);
  }
}

void load_colors(connection *C, vector<Color> &colors) {
  for (const auto &color : colors) {
    add_color(C, color.name);
  }
}

void load_teams(connection *C, vector<Team> &teams) {
  for (const auto &team : teams) {
    add_team(C, team.name, team.state_id, team.color_id, team.wins,
             team.losses);
  }
}

void load_players(connection *C, vector<Player> &players) {
  for (const auto &player : players) {
    add_player(C, player.team_id, player.uniform_num, player.first_name,
               player.last_name, player.mpg, player.ppg, player.rpg, player.apg,
               player.spg, player.bpg);
  }
}

int main(int argc, char *argv[]) {
  // Allocate & initialize a Postgres connection object
  connection *C;

  try {
    // Establish a connection to the database
    // Parameters: database name, user name, user password
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
  } catch (const std::exception &e) {
    cerr << e.what() << std::endl;
    return 1;
  }

  drop_relation(C);
  create_relation(C);

  std::vector<Player> players = parse_player_file("player.txt");
  std::vector<Team> teams = parse_team_file("team.txt");
  std::vector<State> states = parse_state_file("state.txt");
  std::vector<Color> colors = parse_color_file("color.txt");

  exercise(C);

  load_states(C, states);
  load_colors(C, colors);
  load_teams(C, teams);
  load_players(C, players);

  // Close database connection
  C->disconnect();

  return 0;
}
