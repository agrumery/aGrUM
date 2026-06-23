/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <sstream>

#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/multidim/instantiation.h>
#include <agrum/base/variables/labelizedVariable.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Multidim
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct MultidimTestSuite {
    public:
    static void testCreation() {
      gum::MultiDimArray< double > m;
      CHECK_EQ(m.nbrDim(), static_cast< gum::Size >(0));
      CHECK_EQ(m.domainSize(), static_cast< gum::Size >(1));

      gum::LabelizedVariable a("a", "", 4), b("b", "", 5);
      m << a;
      CHECK_EQ(m.nbrDim(), static_cast< gum::Size >(1));
      CHECK_EQ(m.domainSize(), static_cast< gum::Size >(4));
      m << b;
      CHECK_EQ(m.nbrDim(), static_cast< gum::Size >(2));
      CHECK_EQ(m.domainSize(), static_cast< gum::Size >(20));

      gum::MultiDimArray< double > mm = m;
      CHECK_EQ(mm.nbrDim(), static_cast< gum::Size >(2));
      CHECK_EQ(mm.domainSize(), static_cast< gum::Size >(20));
    }   // namespace gum_tests

    static void testMemoryCrash() {
      gum::MultiDimArray< double > m;
      gum::LabelizedVariable*      v[100];

      for (int i = 0; i < 100; i++) {
        std::string name = "x";
        name.append(std::to_string(i));
        v[i] = new gum::LabelizedVariable(name, "x");
      }

      // GCC 16 false positive: inlined feedMultiDimUntilOverflow triggers -Warray-bounds
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
      CHECK_THROWS_AS(feedMultiDimUntilOverflow(v, m), const gum::OutOfBounds&);
#pragma GCC diagnostic pop

      for (int i = 0; i < 100; i++)
        delete (v[i]);
    }

    // MED-2: domainSize multiplication guard in MultiDimImplementation::add()
    static void testDomainSizeOverflow() {
      gum::LabelizedVariable v1("v1", "v1", 3);
      gum::LabelizedVariable v2("v2", "v2", 5);
      gum::LabelizedVariable v3("v3", "v3", 7);

      gum::MultiDimArray< double > m;
      GUM_CHECK_ASSERT_THROWS_NOTHING(m.add(v1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(m.add(v2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(m.add(v3));
      CHECK_EQ(m.domainSize(), gum::Size(3 * 5 * 7));
    }

    private:
    static void feedMultiDimUntilOverflow(gum::LabelizedVariable*       v[],
                                          gum::MultiDimArray< double >& t) {
      t.beginMultipleChanges();

      for (int i = 0; i < 100; i++)
        t << *v[i];

      t.endMultipleChanges();
    }
  };

  GUM_TEST_ACTIF(Creation)
  GUM_TEST_ACTIF(MemoryCrash)
  GUM_TEST_ACTIF(DomainSizeOverflow)
}   // namespace gum_tests
