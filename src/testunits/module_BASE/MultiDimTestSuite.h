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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/multidim/instantiation.h>
#include <agrum/base/variables/labelizedVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Multidim
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct MultidimTestSuite {
    public:
    static void testCreation() {
      gum::MultiDimArray< double > m;
      CHECK((m.nbrDim()) == (static_cast< gum::Size >(0)));
      CHECK((m.domainSize()) == (static_cast< gum::Size >(1)));

      gum::LabelizedVariable a("a", "", 4), b("b", "", 5);
      m << a;
      CHECK((m.nbrDim()) == (static_cast< gum::Size >(1)));
      CHECK((m.domainSize()) == (static_cast< gum::Size >(4)));
      m << b;
      CHECK((m.nbrDim()) == (static_cast< gum::Size >(2)));
      CHECK((m.domainSize()) == (static_cast< gum::Size >(20)));

      gum::MultiDimArray< double > mm = m;
      CHECK((mm.nbrDim()) == (static_cast< gum::Size >(2)));
      CHECK((mm.domainSize()) == (static_cast< gum::Size >(20)));
    }   // namespace gum_tests

    static void testMemoryCrash() {
      gum::MultiDimArray< double > m;
      gum::LabelizedVariable*      v[100];

      for (int i = 0; i < 100; i++) {
        std::string name = "x";
        name.append(std::to_string(i));
        v[i] = new gum::LabelizedVariable(name, "x");
      }

      CHECK_THROWS_AS(feedMultiDimUntilOverflow(v, m), const gum::OutOfBounds&);

      for (int i = 0; i < 100; i++)
        delete (v[i]);
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
}   // namespace gum_tests
