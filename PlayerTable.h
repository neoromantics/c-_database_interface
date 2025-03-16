#include "DBTable.h"

class PlayerTable : public DBTable {
 public:
  void drop(connection *C) const override {
    work W(*C);
    W.exec("DROP TABLE IF EXISTS PLAYER;");
    W.commit();
  }
  void create(connection *C) const override {
    work W(*C);
    W.exec(
        "CREATE TABLE PLAYER("
        "  PLAYER_ID SERIAL PRIMARY KEY, "
        "  TEAM_ID INT, "
        "  UNIFORM_NUM INT, "
        "  FIRST_NAME VARCHAR(50), "
        "  LAST_NAME VARCHAR(50), "
        "  MPG INT, "
        "  PPG INT, "
        "  RPG INT, "
        "  APG INT, "
        "  SPG DECIMAL(3,1), "
        "  BPG DECIMAL(3,1)"
        ");");
    W.commit();
  }
};
