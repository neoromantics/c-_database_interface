#ifndef DBTABLE_H
#define DBTABLE_H

#include <pqxx/pqxx>

using namespace pqxx;

class DBTable {
 public:
  virtual ~DBTable() {}
  virtual void drop(connection *C) const = 0;
  virtual void create(connection *C) const = 0;
};

#endif