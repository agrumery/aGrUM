
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES
 * (@AMU) info_at_agrum_dot_org
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


#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/MN/inference/ShaferShenoyMNInference.h>
#include <agrum/MN/MarkovNet.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/tools/variables/rangeVariable.h>
#include <agrum/tools/variables/discretizedVariable.h>

namespace gum_tests {
  class ShaferShenoyMNTestSuite: public CxxTest::TestSuite {
    public:
    void testConstructor() {
      auto mn = gum::MarkovNet< double >::fastPrototype("A-B-C;C-D;C-E-F");
      gum::ShaferShenoyMNInference< double > ie(&mn);
    }

    void testSimpleInference() {
      auto mn = gum::MarkovNet< double >::fastPrototype("A-B-C;C-D;C-E-F");
      gum::ShaferShenoyMNInference< double > ie(&mn);
      ie.makeInference();
    }

    void testCompareInferenceTree() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D<-E;B->F;D->G;");
      gum::LazyPropagation< double > iebn(&bn);
      iebn.makeInference();

      auto mn = gum::MarkovNet< double >::fromBN(bn);
      gum::ShaferShenoyMNInference< double > iemn(&mn);
      iemn.makeInference();

      for (const auto n: bn.nodes()) {
        const std::string&              name = bn.variable(n).name();
        const gum::Potential< double >& postbn = iebn.posterior(name);

        gum::Potential< double > postmn;
        postmn.add(bn.variable(n));
        postmn.fillWith(iemn.posterior(name));   // postmn using bn variable

        TS_ASSERT_LESS_THAN((postbn - postmn).abs().max(), 1e-7);
      }
    }

    void testCompareInferenceDAG() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D<-E;B->F;D->F;");
      gum::LazyPropagation< double > iebn(&bn);
      iebn.makeInference();

      auto mn = gum::MarkovNet< double >::fromBN(bn);
      gum::ShaferShenoyMNInference< double > iemn(&mn);
      iemn.makeInference();

      for (const auto n: bn.nodes()) {
        const std::string&              name = bn.variable(n).name();
        const gum::Potential< double >& postbn = iebn.posterior(name);

        gum::Potential< double > postmn;
        postmn.add(bn.variable(n));
        postmn.fillWith(iemn.posterior(name));   // postmn using bn variable

        TS_ASSERT_LESS_THAN((postbn - postmn).abs().max(), 1e-7);
      }
    }


    void testCompareInferenceTreeWithEvidence() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D<-E;B->F;D->G;");
      gum::LazyPropagation< double > iebn(&bn);
      iebn.addEvidence("B", 1);
      iebn.makeInference();

      auto mn = gum::MarkovNet< double >::fromBN(bn);
      gum::ShaferShenoyMNInference< double > iemn(&mn);
      iemn.addEvidence("B", 1);
      iemn.makeInference();

      for (const auto n: bn.nodes()) {
        const std::string&              name = bn.variable(n).name();
        const gum::Potential< double >& postbn = iebn.posterior(name);

        gum::Potential< double > postmn;
        postmn.add(bn.variable(n));
        postmn.fillWith(iemn.posterior(name));   // postmn using bn variable

        TS_ASSERT_LESS_THAN((postbn - postmn).abs().max(), 1e-7);
      }
    }

    void testCompareInferenceDAGWithEvidence() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D<-E;B->F;D->F;");
      gum::LazyPropagation< double > iebn(&bn);
      iebn.addEvidence("B", 1);
      iebn.makeInference();

      auto mn = gum::MarkovNet< double >::fromBN(bn);
      gum::ShaferShenoyMNInference< double > iemn(&mn);
      iemn.addEvidence("B", 1);
      iemn.makeInference();

      for (const auto n: bn.nodes()) {
        const std::string&              name = bn.variable(n).name();
        const gum::Potential< double >& postbn = iebn.posterior(name);

        gum::Potential< double > postmn;
        postmn.add(bn.variable(n));
        postmn.fillWith(iemn.posterior(name));   // postmn using bn variable

        TS_ASSERT_LESS_THAN((postbn - postmn).abs().max(), 1e-8);
      }
    }

    void testClassicalInference() {
      auto mn = gum::MarkovNet< double >::fastPrototype("A-B-C;C-D;D-E-F;F-A");
      gum::ShaferShenoyMNInference< double > iemn(&mn);
      iemn.addEvidence("B", 1);
      iemn.makeInference();

      for (const auto n: mn.nodes()) {
        TS_ASSERT_DELTA(iemn.posterior(n).sum(),1.0,1e-8);
      }
    }

    void testSeparationInInference() {
      auto mn = gum::MarkovNet< double >::fastPrototype("A-B-C;C-D;D-E-F;F-A");
      gum::ShaferShenoyMNInference< double > iemn(&mn);
      iemn.addEvidence("A", 0);
      iemn.addEvidence("D", 1);
      iemn.makeInference();
      gum::Potential< double > Cwithout_evB(iemn.posterior("C"));
      gum::Potential< double > Ewithout_evB(iemn.posterior("E"));

      iemn.eraseAllEvidence();
      iemn.addEvidence("A", 0);
      iemn.addEvidence("D", 1);
      iemn.addEvidence("B", 0);
      iemn.makeInference();
      gum::Potential< double > Cwith_evB0(iemn.posterior("C"));
      gum::Potential< double > Ewith_evB0(iemn.posterior("E"));

      iemn.eraseAllEvidence();
      iemn.addEvidence("A", 0);
      iemn.addEvidence("D", 1);
      iemn.addEvidence("B", 1);
      iemn.makeInference();
      gum::Potential< double > Cwith_evB1(iemn.posterior("C"));
      gum::Potential< double > Ewith_evB1(iemn.posterior("E"));

      TS_ASSERT_LESS_THAN(-(Cwithout_evB - Cwith_evB0).abs().max(), -1e-8);
      TS_ASSERT_LESS_THAN(-(Cwithout_evB - Cwith_evB1).abs().max(), -1e-8);
      TS_ASSERT_LESS_THAN(-(Cwith_evB1 - Cwith_evB0).abs().max(), -1e-8);

      TS_ASSERT_LESS_THAN((Ewithout_evB - Ewith_evB0).abs().max(), 1e-8);
      TS_ASSERT_LESS_THAN((Ewithout_evB - Ewith_evB1).abs().max(), 1e-8);
      TS_ASSERT_LESS_THAN((Ewith_evB1 - Ewith_evB0).abs().max(), 1e-8);
    }
    void testWeirdInference() {
      auto mn = gum::MarkovNet< double >::fastPrototype("A;B;C;D;E;F");
      gum::ShaferShenoyMNInference< double > iemn(&mn);
      iemn.addEvidence("B", 1);
      iemn.makeInference();

      for (const auto n: mn.nodes()) {
        TS_ASSERT_DELTA(iemn.posterior(n).sum(),1.0,1e-8);
      }
    }

    void testIncrementalInference() {
      auto mn = gum::MarkovNet< double >::fastPrototype("A-B-C;C-D;D-E-F;F-A");
      gum::ShaferShenoyMNInference<double> ie(&mn);
      ie.addEvidence("A", 0);
      ie.addEvidence("D", 1);
      ie.addEvidence("B", 0);
      ie.makeInference();

      gum::ShaferShenoyMNInference<double> ie2(&mn);
      ie2.addEvidence("A", 1);
      ie2.addEvidence("D", 1);
      ie2.addEvidence("B", 1);
      ie2.makeInference();
      ie2.chgEvidence("A", 0);
      ie2.chgEvidence("B", 0);
      ie2.makeInference();

      for(const auto n:mn.nodes()) {
        TS_ASSERT_LESS_THAN((ie2.posterior(n)-ie.posterior(n)).abs().max(),1e-8);
      }
    }
  };
}   // namespace gum_tests