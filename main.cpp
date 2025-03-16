#include "DatabaseManager.h"

int main() {
  try {
    DatabaseManager dbManager(
        "dbname=ACC_BBALL user=postgres password=passw0rd");
    cout << "Opened database successfully: "
         << dbManager.getConnection()->dbname() << endl;

    dbManager.setupTables();

    dbManager.loadAndInsertRecords();

    dbManager.runTests();
  } catch (const exception &e) {
    cerr << e.what() << endl;
    return 1;
  }
  return 0;
}
