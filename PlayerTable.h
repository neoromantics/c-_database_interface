#ifndef PLAYERTABLE_H
#define PLAYERTABLE_H

#include <pqxx/pqxx>

#include "DBTable.h"

using namespace pqxx;

class PlayerTable : public DBTable {
 public:
  void drop(connection *C) const override;
  void create(connection *C) const override;
};

#endif
