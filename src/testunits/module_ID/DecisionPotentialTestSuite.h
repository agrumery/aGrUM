/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
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

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/ID/inference/tools/decisionPotential.h>
#include <agrum/ID/influenceDiagram.h>

namespace gum_tests {

  class DecisionPotentialTestSuite: public CxxTest::TestSuite {
    public:
    void testConstruction() {
      gum::DecisionPotential< double > d1, d2;
      TS_ASSERT_EQUALS(d1, d2);
      TS_ASSERT_EQUALS(d1, d1 * d2);

      gum::DecisionPotential< double > d3(std::move(d2));

      gum::LabelizedVariable   a("a", "first var", 2), b("b", "second var", 4);
      gum::LabelizedVariable   c("c", "first var", 2), d("d", "second var", 4);
      gum::Potential< double > P1, U1;
      P1 << a << b;
      U1 << c << d;

      gum::DecisionPotential< double > d4(P1, U1);
      gum::DecisionPotential< double > d5(d4);
      TS_ASSERT_EQUALS(d4, d5);

      gum::DecisionPotential< double > d6 = d5;
      TS_ASSERT_EQUALS(d6, d4);

      gum::DecisionPotential< double > d7(std::move(d4));
      TS_ASSERT_EQUALS(d7, d6);

      d1 = std::move(d7);
      TS_ASSERT_EQUALS(d1, d6);

      d5 = d1;
      TS_ASSERT_EQUALS(d1, d5);

      d4 = d1;
      TS_ASSERT_EQUALS(d1, d4);
    }

    void testConstruction2() {
      auto infdiag = gum::InfluenceDiagram< double >::fastPrototype("A->$B<-*C");
      infdiag.cpt("A").fillWith({0.3, 0.7});
      infdiag.utility("B").fillWith({100, 10, 30, 70});
      gum::DecisionPotential< double > d1;
      d1.insertProba(infdiag.cpt("A"));
      d1.insertUtility(infdiag.utility("B"));

      auto res = d1 ^ std::vector< std::string >({"C"});
      TS_ASSERT_EQUALS(res.probPot, gum::Potential< double >().fillWith(1));
      TS_ASSERT_EQUALS(
         res.utilPot,
         (gum::Potential< double >() << infdiag.variableFromName("C")).fillWith({37, 58}));
    }

    void testEqualities() {
      auto infdiag = gum::InfluenceDiagram< double >::fastPrototype("A->$B<-*C");
      infdiag.cpt("A").fillWith({1.0, 0.0});
      infdiag.utility("B").fillWith({100, 10, 30, 70});
      gum::DecisionPotential< double > d1;
      d1.insertProba(infdiag.cpt("A"));
      d1.insertUtility(infdiag.utility("B"));

      infdiag.utility("B").fillWith({100, 70, 30, 10});
      gum::DecisionPotential< double > d2;
      d2.insertProba(infdiag.cpt("A"));
      d2.insertUtility(infdiag.utility("B"));

      TS_ASSERT_EQUALS(d1.probPot, d2.probPot);
      TS_ASSERT_DIFFERS(d1.utilPot, d2.utilPot);
      TS_ASSERT_EQUALS(d1, d2);   // d1 and d2 differ for utility with proba is 0
    }

    void testSuperSetForMarginalization() {
      auto infdiag = gum::InfluenceDiagram< double >::fastPrototype("D<-A->$B<-*C");
      infdiag.cpt("A").fillWith({0.3, 0.7});
      infdiag.cpt("D").fillWith({0.1, 0.9, 0.9, 0.1});
      infdiag.utility("B").fillWith({100, 10, 30, 70});
      gum::DecisionPotential< double > d1;
      d1.insertProba(infdiag.cpt("A"));
      d1.insertUtility(infdiag.utility("B"));

      gum::Set< const gum::DiscreteVariable* > vars;
      vars.insert(&infdiag.variableFromName("C"));
      auto res1 = d1 ^ vars;

      // we add a variable not belonging tod1
      vars.insert(&infdiag.variableFromName("D"));
      auto res2 = d1 ^ vars;

      TS_ASSERT_EQUALS(res1, res2);
    }
  };
}   // namespace gum_tests
