#ifndef STATETABLE_H
#define STATETABLE_H

#include <pqxx/pqxx>

#include "DBTable.h"

using namespace pqxx;

class StateTable : public DBTable {
 public:
  void drop(connection *C) const override;
  void create(connection *C) const override;
};

#endif  // STATETABLE_H
