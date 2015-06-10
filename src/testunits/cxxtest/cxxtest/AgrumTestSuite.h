#ifndef AGRUM_TEST_SUITE_H
#define AGRUM_TEST_SUITE_H

#include <agrum/config.h>

#include <cxxtest/TestSuite.h>

#define TS_GUM_ASSERT_THROWS_NOTHING(x)                                             \
  TS_ASSERT_THROWS_NOTHING(try { x; } catch (gum::Exception & e) {                  \
    GUM_SHOWERROR(e);                                                               \
    TS_FAIL("gum::Exception thrown");                                               \
  })

#endif // AGRUM_TEST_SUITE_H
