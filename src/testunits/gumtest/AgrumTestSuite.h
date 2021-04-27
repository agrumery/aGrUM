
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef AGRUM_TEST_SUITE_H
#define AGRUM_TEST_SUITE_H

#include <agrum/config.h>
#include <gumtest/testsuite_utils.h>

#include <cxxtest/TestSuite.h>

#define TS_GUM_ASSERT_THROWS_NOTHING(block)      \
  TS_ASSERT_THROWS_NOTHING(                      \
     try { block; } catch (gum::Exception & e) { \
       GUM_TRACE_NEWLINE                         \
       GUM_SHOWERROR(e);                         \
       TS_FAIL("gum::Exception thrown");         \
     })

#define TS_GUM_POTENTIAL_DELTA(p1, p2, delta) \
  TS_ASSERT_LESS_THAN(((p1) - (p2)).abs().max(), delta)

#define TS_GUM_POTENTIAL_SHOW_DELTA(p1, p2, delta)           \
  {                                                          \
    GUM_TRACE_NEWLINE GUM_TRACE_VAR(p1) GUM_TRACE_VAR(p2)    \
       TS_ASSERT_LESS_THAN(((p1) - (p2)).abs().max(), delta) \
  }

#define TS_GUM_SMALL_ERROR      (1e-5)
#define TS_GUM_VERY_SMALL_ERROR (1e-10)

#endif   // AGRUM_TEST_SUITE_H
