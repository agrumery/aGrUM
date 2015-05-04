#ifndef TESTSUITE_UTILS_H
#define TESTSUITE_UTILS_H

#include <iostream>
#include <fstream>

#define xstrfy(s) strfy(s)
#define strfy(x) #x

#define GET_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/" x
#define GET_PATH_XSTR(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/" xstrfy(x)

namespace gum_tests {
  void test_waiting(int s) {
    const char waiter[] = "|/-\\";
    std::cout << '\b' << waiter[s % 4];
    std::flush(std::cout);
  }

  void end_test_waiting() {
    std::cout << '\b';
    std::flush(std::cout);
  }

  void str2file(const std::string &filename, const std::string &message) {
    std::ofstream outFile;
    outFile.open(filename);
    outFile << message << std::endl;
    outFile.close();
  }
}
#endif // TESTSUITE_UTILS_H
