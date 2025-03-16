#ifndef DBRECORD_H
#define DBRECORD_H

#include <pqxx/pqxx>
#include <string>

#include "query_funcs.h"
using namespace std;
using namespace pqxx;

class DBRecord {
 public:
  virtual ~DBRecord() {}
  virtual void insertIntoDB(connection *C) const = 0;
};

#endif