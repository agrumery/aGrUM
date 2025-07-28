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


#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/ID/inference/tools/decisionTensor.h>
#include <agrum/ID/influenceDiagram.h>

namespace gum_tests {

  class [[maybe_unused]] DecisionTensorTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Construction) {
      gum::DecisionTensor< double > d1, d2;
      TS_ASSERT_EQUALS(d1, d2)
      TS_ASSERT_EQUALS(d1, d1 * d2)

      gum::DecisionTensor< double > d3(std::move(d2));

      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4);
      gum::LabelizedVariable c("c", "first var", 2), d("d", "second var", 4);
      gum::Tensor< double >  P1, U1;
      P1 << a << b;
      U1 << c << d;

      gum::DecisionTensor< double > d4(P1, U1);
      gum::DecisionTensor< double > d5(d4);
      TS_ASSERT_EQUALS(d4, d5)

      gum::DecisionTensor< double > d6 = d5;
      TS_ASSERT_EQUALS(d6, d4)

      gum::DecisionTensor< double > d7(std::move(d4));
      TS_ASSERT_EQUALS(d7, d6)

      d1 = std::move(d7);
      TS_ASSERT_EQUALS(d1, d6)

      d5 = d1;
      TS_ASSERT_EQUALS(d1, d5)

      d4 = d1;
      TS_ASSERT_EQUALS(d1, d4)
    }

    GUM_ACTIVE_TEST(Construction2) {
      auto infdiag = gum::InfluenceDiagram< double >::fastPrototype("A->$B<-*C");
      infdiag.cpt("A").fillWith({0.3, 0.7});
      infdiag.utility("B").fillWith({100, 10, 30, 70});
      gum::DecisionTensor< double > d1;
      d1.insertProba(infdiag.cpt("A"));
      d1.insertUtility(infdiag.utility("B"));

      auto res = d1 ^ std::vector< std::string >({"C"});
      TS_ASSERT_EQUALS(res.probPot, gum::Tensor< double >().fillWith(1))
      TS_ASSERT_EQUALS(
          res.utilPot,
          (gum::Tensor< double >() << infdiag.variableFromName("C")).fillWith({37, 58}));
    }

    GUM_ACTIVE_TEST(Equalities) {
      auto infdiag = gum::InfluenceDiagram< double >::fastPrototype("A->$B<-*C");
      infdiag.cpt("A").fillWith({1.0, 0.0});
      infdiag.utility("B").fillWith({100, 10, 30, 70});
      gum::DecisionTensor< double > d1;
      d1.insertProba(infdiag.cpt("A"));
      d1.insertUtility(infdiag.utility("B"));

      infdiag.utility("B").fillWith({100, 70, 30, 10});
      gum::DecisionTensor< double > d2;
      d2.insertProba(infdiag.cpt("A"));
      d2.insertUtility(infdiag.utility("B"));

      TS_ASSERT_EQUALS(d1.probPot, d2.probPot)
      TS_ASSERT_DIFFERS(d1.utilPot, d2.utilPot)
      TS_ASSERT_EQUALS(d1, d2);   // d1 and d2 differ for utility with proba is 0
    }

    GUM_ACTIVE_TEST(SuperSetForMarginalization) {
      auto infdiag = gum::InfluenceDiagram< double >::fastPrototype("D<-A->$B<-*C");
      infdiag.cpt("A").fillWith({0.3, 0.7});
      infdiag.cpt("D").fillWith({0.1, 0.9, 0.9, 0.1});
      infdiag.utility("B").fillWith({100, 10, 30, 70});
      gum::DecisionTensor< double > d1;
      d1.insertProba(infdiag.cpt("A"));
      d1.insertUtility(infdiag.utility("B"));

      gum::VariableSet vars;
      vars.insert(&infdiag.variableFromName("C"));
      auto res1 = d1 ^ vars;

      // we add a variable not belonging tod1
      vars.insert(&infdiag.variableFromName("D"));
      auto res2 = d1 ^ vars;

      TS_ASSERT_EQUALS(res1, res2)
    }
  };
}   // namespace gum_tests
