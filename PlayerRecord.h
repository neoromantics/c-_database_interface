#include "DBRecord.h"

class PlayerRecord : public DBRecord {
 public:
  int team_id, uniform_num;
  string first_name, last_name;
  int mpg, ppg, rpg, apg;
  double spg, bpg;
  PlayerRecord(int tid, int unum, const string &fn, const string &ln, int mpg,
               int ppg, int rpg, int apg, double spg, double bpg)
      : team_id(tid),
        uniform_num(unum),
        first_name(fn),
        last_name(ln),
        mpg(mpg),
        ppg(ppg),
        rpg(rpg),
        apg(apg),
        spg(spg),
        bpg(bpg) {}
  void insertIntoDB(connection *C) const override {
    add_player(C, team_id, uniform_num, first_name, last_name, mpg, ppg, rpg,
               apg, spg, bpg);
  }
};