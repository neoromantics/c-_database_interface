#include "StateTable.h"

void StateTable::drop(connection *C) const {
  work W(*C);
  W.exec("DROP TABLE IF EXISTS STATE;");
  W.commit();
}

void StateTable::create(connection *C) const {
  work W(*C);
  W.exec(
      "CREATE TABLE STATE("
      "  STATE_ID SERIAL PRIMARY KEY, "
      "  NAME VARCHAR(50)"
      ");");
  W.commit();
}
