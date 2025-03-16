#include "DBRecord.h"

class StateRecord : public DBRecord {
 public:
  string name;
  StateRecord(const string &n) : name(n) {}
  void insertIntoDB(connection *C) const override { add_state(C, name); }
};
