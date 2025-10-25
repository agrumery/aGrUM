/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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


#include <gumtest/AgrumTestSuite.h>

#include <agrum/BN/algorithms/BayesBall.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/PRM/inference/structuredBayesBall.h>
#include <agrum/PRM/inference/SVE.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum_tests {
  class GUM_TEST_SUITE(StructuredBayesBall) {
    gum::prm::PRM< double >*          prm;
    gum::prm::PRMInference< double >* prm_inf;
    gum::prm::PRMSystem< double >*    sys;
    gum::prm::PRM< double >*          small;
    gum::prm::PRMInference< double >* small_inf;
    gum::prm::PRMSystem< double >*    small_sys;

    public:
    void setUp() {
      {
        gum::prm::o3prm::O3prmReader< double > reader;
        reader.readFile(GET_RESSOURCES_PATH("o3prm/inference.o3prm"));
        prm     = reader.prm();
        sys     = &(prm->getSystem("aSys"));
        prm_inf = new gum::prm::SVE< double >(*prm, *sys);
      }
      {
        gum::prm::o3prm::O3prmReader< double > reader;
        reader.readFile(GET_RESSOURCES_PATH("o3prm/printers_systems.o3prm"));
        small     = reader.prm();
        small_sys = &(small->getSystem("smallSys"));
        small_inf = new gum::prm::SVE< double >(*small, *small_sys);
      }
    }

    void tearDown() {
      delete prm;
      delete prm_inf;
      delete small;
      delete small_inf;
    }

    GUM_ACTIVE_TEST(Constructors) {
      gum::prm::StructuredBayesBall< double >* bb = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(bb = new gum::prm::StructuredBayesBall< double >(*prm_inf))
      TS_GUM_ASSERT_THROWS_NOTHING(delete bb)
      TS_GUM_ASSERT_THROWS_NOTHING(bb = new gum::prm::StructuredBayesBall< double >(*small_inf))
      TS_GUM_ASSERT_THROWS_NOTHING(delete bb)
    }

    /// Checking that when a root is queried and there is no evidence, the
    /// requisite nodes set contains only the root node.
    GUM_ACTIVE_TEST(RootsNoObs) {
      gum::prm::StructuredBayesBall< double >* bb = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(bb = new gum::prm::StructuredBayesBall< double >(*prm_inf))

      for (auto i = sys->begin(); i != sys->end(); ++i) {
        for (auto a = (*(i.val())).begin(); a != (*(i.val())).end(); ++a) {
          if ((*(i.val())).type().containerDag().parents((*(a.val())).id()).empty()) {
            TS_GUM_ASSERT_THROWS_NOTHING(bb->compute(i.val(), (*(a.val())).id()))

            for (auto j = sys->begin(); j != sys->end(); ++j) {
              if ((j.val()) != (i.val())) {
                TS_ASSERT(!bb->exists(j.val()))
              } else if (bb->exists(j.val())) {
                TS_ASSERT_EQUALS(bb->requisiteNodes(j.val()).size(), static_cast< gum::Size >(1))
                TS_ASSERT(bb->requisiteNodes(j.val()).contains((*(a.val())).id()))
              } else {
                TS_ASSERT(false)
              }
            }
          }
        }
      }

      TS_GUM_ASSERT_THROWS_NOTHING(delete bb)
    }

    /// Checking that when a root is queried and there is no evidence, the
    /// requisite nodes set contains only the root node.
    GUM_ACTIVE_TEST(RootsNoObsSmall) {
      gum::prm::StructuredBayesBall< double >* bb = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(bb = new gum::prm::StructuredBayesBall< double >(*small_inf))

      for (auto i = small_sys->begin(); i != small_sys->end(); ++i) {
        for (auto a = (*(i.val())).begin(); a != (*(i.val())).end(); ++a) {
          if ((*(i.val())).type().containerDag().parents((*(a.val())).id()).empty()) {
            TS_GUM_ASSERT_THROWS_NOTHING(bb->compute(i.val(), (*(a.val())).id()))

            for (gum::prm::PRMSystem< double >::iterator j = small_sys->begin();
                 j != small_sys->end();
                 ++j) {
              if ((j.val()) != (i.val())) {
                TS_ASSERT(!bb->exists(j.val()))
              } else if (bb->exists(j.val())) {
                TS_ASSERT_EQUALS(bb->requisiteNodes(j.val()).size(), static_cast< gum::Size >(1))
                TS_ASSERT(bb->requisiteNodes(j.val()).contains((*(a.val())).id()))
              } else {
                TS_ASSERT(false)
              }
            }
          }
        }
      }

      TS_GUM_ASSERT_THROWS_NOTHING(delete bb)
    }
  };

}   // namespace gum_tests
