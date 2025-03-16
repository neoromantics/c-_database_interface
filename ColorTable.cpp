#include "ColorTable.h"

void ColorTable::drop(connection *C) const {
  work W(*C);
  W.exec("DROP TABLE IF EXISTS COLOR;");
  W.commit();
}

void ColorTable::create(connection *C) const {
  work W(*C);
  W.exec(
      "CREATE TABLE COLOR("
      "  COLOR_ID SERIAL PRIMARY KEY, "
      "  NAME VARCHAR(50)"
      ");");
  W.commit();
}
