#include "DBTable.h"

class TeamTable : public DBTable {
 public:
  void drop(connection *C) const override {
    work W(*C);
    W.exec("DROP TABLE IF EXISTS TEAM;");
    W.commit();
  }
  void create(connection *C) const override {
    work W(*C);
    W.exec(
        "CREATE TABLE TEAM("
        "  TEAM_ID SERIAL PRIMARY KEY, "
        "  NAME VARCHAR(50), "
        "  STATE_ID INT, "
        "  COLOR_ID INT, "
        "  WINS INT, "
        "  LOSSES INT"
        ");");
    W.commit();
  }
};