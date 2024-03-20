#include <iostream>

#include "server/simpledb.h"

namespace simpledb {
void PlannerStudentTest() {
  // SimpleDB db {"studentdb"};

  std::cout << "No studentdb instance created yet - wait for SimpleDB clients "
               "to create\n";
}
}  // namespace simpledb

int main() {
  simpledb::PlannerStudentTest();

  return 0;
}
