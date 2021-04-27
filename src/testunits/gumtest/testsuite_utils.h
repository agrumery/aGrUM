#ifndef TESTSUITE_UTILS_H
#define TESTSUITE_UTILS_H

#include <fstream>
#include <iostream>

#define xstrfy( s ) strfy( s )
#define strfy( x ) #x

#define GET_RESSOURCES_PATH( relpath ) xstrfy( GUM_SRC_PATH ) "/testunits/ressources/" relpath
#define XGET_RESSOURCES_PATH( x ) \
  xstrfy( GUM_SRC_PATH ) "/testunits/ressources/" xstrfy( x )

namespace gum_tests {
  const std::string waiter[]{"[*  ]", "[ * ]", "[  *]", "[ * ]"};
  const std::string backst = "\b\b\b\b\b";

  void begin_test_waiting() {
    std::cout << waiter[0];
    std::flush( std::cout );
  }

  void test_waiting( int s = -1 ) {
    static int ss = 0;

    if ( s == -1 ) s = ss++;
    std::cout << backst << waiter[s % 4];
    std::flush( std::cout );
  }

  void end_test_waiting() {
    std::cout << backst;
    std::flush( std::cout );
  }

  void str2file( const std::string& filename, const std::string& message ) {
    std::ofstream outFile;
    outFile.open( filename );
    outFile << message << std::endl;
    outFile.close();
  }
}
#endif  // TESTSUITE_UTILS_H
