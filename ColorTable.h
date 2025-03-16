#ifndef COLORTABLE_H
#define COLORTABLE_H

#include <pqxx/pqxx>

#include "DBTable.h"

using namespace pqxx;

class ColorTable : public DBTable {
 public:
  void drop(connection *C) const override;
  void create(connection *C) const override;
};

#endif
