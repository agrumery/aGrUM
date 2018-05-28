#ifndef AGRUM_TEST_SUITE_H
#define AGRUM_TEST_SUITE_H

// TRACE for AGRUM are ON
#define GUM_TRACE_ON

#include <agrum/config.h>
#include <cxxtest/testsuite_utils.h>

#include <cxxtest/TestSuite.h>

#define TS_GUM_ASSERT_THROWS_NOTHING(x)                            \
  TS_ASSERT_THROWS_NOTHING(try { x; } catch (gum::Exception & e) { \
    GUM_TRACE_NEWLINE                                              \
    GUM_SHOWERROR(e);                                              \
    TS_FAIL("gum::Exception thrown");                              \
  })
#endif   // AGRUM_TEST_SUITE_H
