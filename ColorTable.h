#include "DBTable.h"

class ColorTable : public DBTable {
 public:
  void drop(connection *C) const override {
    work W(*C);
    W.exec("DROP TABLE IF EXISTS COLOR;");
    W.commit();
  }
  void create(connection *C) const override {
    work W(*C);
    W.exec(
        "CREATE TABLE COLOR("
        "  COLOR_ID SERIAL PRIMARY KEY, "
        "  NAME VARCHAR(50)"
        ");");
    W.commit();
  }
};