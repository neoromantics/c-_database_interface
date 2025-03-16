#include "DBRecord.h"
class TeamRecord : public DBRecord {
 public:
  string name;
  int state_id, color_id, wins, losses;
  TeamRecord(const string &n, int sid, int cid, int w, int l)
      : name(n), state_id(sid), color_id(cid), wins(w), losses(l) {}
  void insertIntoDB(connection *C) const override {
    add_team(C, name, state_id, color_id, wins, losses);
  }
};
