#ifndef AGRUM_TEST_SUITE_H
#define AGRUM_TEST_SUITE_H

// TRACE for AGRUM are ON
#define GUM_TRACE_ON

#include <agrum/config.h>
#include <cxxtest/testsuite_utils.h>

#include <cxxtest/TestSuite.h>

#define TS_GUM_ASSERT_THROWS_NOTHING( x )                             \
  TS_ASSERT_THROWS_NOTHING( try { x; } catch ( gum::Exception & e ) { \
    GUM_TRACE_NEWLINE                                                 \
    GUM_SHOWERROR( e );                                               \
    TS_FAIL( "gum::Exception thrown" );                               \
  } )

#define GUM_APPROX_TEST_BEGIN_ITERATION \
  bool res = false;                          \
  (void)res;                                 \
  for ( int ii = 0; ii < 10; ii++ ) {

#define GUM_APPROX_TEST_END_ITERATION( ERRMAX )   \
  if ( __compareInference( bn, lazy, inf, ERRMAX ) ) { \
    res = true;                                        \
    break;                                             \
  }                                                    \
  std::cout << "![" << __LINE__ << "]"<<std::flush;                \
  if ( ii == 10 ) TS_FAIL( "even with 10 tries." );    \
  }
#endif  // AGRUM_TEST_SUITE_H
