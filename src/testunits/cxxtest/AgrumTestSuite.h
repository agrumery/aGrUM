#ifndef AGRUM_TEST_SUITE_H
#define AGRUM_TEST_SUITE_H

#include <agrum/config.h>
#include <cxxtest/testsuite_utils.h>

#include <cxxtest/TestSuite.h>

#define TS_GUM_ASSERT_THROWS_NOTHING(x)      \
  TS_ASSERT_THROWS_NOTHING(                  \
     try { x; } catch (gum::Exception & e) { \
       GUM_TRACE_NEWLINE                     \
       GUM_SHOWERROR(e);                     \
       TS_FAIL("gum::Exception thrown");     \
     })

#define TS_GUM_POTENTIAL_DELTA(x, y, delta) \
  TS_ASSERT_LESS_THAN(((x) - (y)).abs().max(), delta)

#define TS_GUM_POTENTIAL_SHOW_DELTA(x, y, delta)           \
  {                                                        \
    GUM_TRACE_NEWLINE GUM_TRACE_VAR(x) GUM_TRACE_VAR(y)    \
       TS_ASSERT_LESS_THAN(((x) - (y)).abs().max(), delta) \
  }

#define TS_GUM_SMALL_ERROR (1e-5)
#define TS_GUM_VERY_SMALL_ERROR (1e-10)

#endif   // AGRUM_TEST_SUITE_H
