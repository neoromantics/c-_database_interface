#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Color {
 public:
  int color_id;
  std::string name;

  Color(int cid, const std::string &name) : color_id(cid), name(name) {}
};
