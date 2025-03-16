#include "DBRecord.h"

class ColorRecord : public DBRecord {
 public:
  string name;
  ColorRecord(const string &n) : name(n) {}
  void insertIntoDB(connection *C) const override { add_color(C, name); }
};
