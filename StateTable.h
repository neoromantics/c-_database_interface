#include "DBTable.h"

using namespace std;
using namespace pqxx;

class StateTable : public DBTable {
 public:
  void drop(connection *C) const override {
    work W(*C);
    W.exec("DROP TABLE IF EXISTS STATE;");
    W.commit();
  }
  void create(connection *C) const override {
    work W(*C);
    W.exec(
        "CREATE TABLE STATE("
        "  STATE_ID SERIAL PRIMARY KEY, "
        "  NAME VARCHAR(50)"
        ");");
    W.commit();
  }
};