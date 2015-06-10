/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/BN/BayesNet.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/BN/inference/lazyPropagation.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class LazyInferenceTestSuite : public CxxTest::TestSuite {
    public:
    gum::BayesNet<float> *bn;
    gum::Id i1, i2, i3, i4, i5;
    gum::Potential<float> *e_i1, *e_i4;

    void setUp() {
      bn = new gum::BayesNet<float>();

      gum::LabelizedVariable n1("1", "", 2), n2("2", "", 2), n3("3", "", 2);
      gum::LabelizedVariable n4("4", "", 2), n5("5", "", 3);

      i1 = bn->add(n1);
      i2 = bn->add(n2);
      i3 = bn->add(n3);
      i4 = bn->add(n4);
      i5 = bn->add(n5);

      bn->addArc(i1, i3);
      bn->addArc(i1, i4);
      bn->addArc(i3, i5);
      bn->addArc(i4, i5);
      bn->addArc(i2, i4);
      bn->addArc(i2, i5);

      e_i1 = new gum::Potential<float>();
      (*e_i1) << bn->variable(i1);
      e_i1->fill((float)0);
      gum::Instantiation inst_1(*e_i1);
      inst_1.chgVal(bn->variable(i1), 0);
      e_i1->set(inst_1, (float)1);

      e_i4 = new gum::Potential<float>();
      (*e_i4) << bn->variable(i4);
      e_i4->fill((float)0);
      gum::Instantiation inst_4(*e_i4);
      inst_4.chgVal(bn->variable(i4), 1);
      e_i4->set(inst_4, (float)1);
    }

    void tearDown() {
      delete bn;
      delete e_i1;
      delete e_i4;
    }

    // Testing when there is no evidence
    void testCreationAndInference() {
      fill(*bn);
      // Testing the inference
      gum::LazyPropagation<float> *inf = 0;
      TS_ASSERT_THROWS_NOTHING(inf = new gum::LazyPropagation<float>(*bn));
      TS_ASSERT_THROWS_NOTHING(inf->makeInference());

      if (inf != 0) {
        TS_ASSERT_THROWS_NOTHING(delete inf);
      }
    }

    void testMarginal() {
      fill(*bn);
      gum::LazyPropagation<float> inf(*bn);

      TS_ASSERT_THROWS_NOTHING(inf.makeInference());
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i1));
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i2));
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i3));
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i4));
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i5));
    }

    void testMarginalWithEvidence() {
      fill(*bn);
      gum::List<const gum::Potential<float> *> e_list;
      e_list.insert(e_i1);
      e_list.insert(e_i4);

      gum::LazyPropagation<float> inf(*bn);

      TS_ASSERT_THROWS_NOTHING(inf.insertEvidence(e_list));

      TS_ASSERT_THROWS_NOTHING(inf.makeInference());
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i1));
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i2));
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i3));
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i4));
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i5));

      gum::LazyPropagation<float> inf2(*bn);

      /* addHardEvidece : memore leak */
      TS_ASSERT_THROWS_NOTHING(inf2.addHardEvidence(i1, 0));
      TS_ASSERT_THROWS_NOTHING(inf2.addHardEvidence(i4, 1));

      TS_ASSERT_THROWS_NOTHING(inf2.makeInference());
      TS_ASSERT_THROWS_NOTHING(inf2.posterior(i1));
      TS_ASSERT_THROWS_NOTHING(inf2.posterior(i2));
      TS_ASSERT_THROWS_NOTHING(inf2.posterior(i3));
      TS_ASSERT_THROWS_NOTHING(inf2.posterior(i4));
      TS_ASSERT_THROWS_NOTHING(inf2.posterior(i5));

      TS_ASSERT(inf.posterior(i1) == inf2.posterior(i1));
      TS_ASSERT(inf.posterior(i2) == inf2.posterior(i2));
      TS_ASSERT(inf.posterior(i3) == inf2.posterior(i3));
      TS_ASSERT(inf.posterior(i4) == inf2.posterior(i4));
      TS_ASSERT(inf.posterior(i5) == inf2.posterior(i5));
    }

    // Testing when there is no evidence
    void testJoint() {
      fill(*bn);
      // Testing the inference
      gum::LazyPropagation<float> inf(*bn);
      gum::NodeSet nodeset;
      nodeset.insert(2);
      nodeset.insert(4);

      gum::Potential<float> *pot = 0;
      TS_ASSERT_THROWS_NOTHING(pot = inf.joint(nodeset));

      if (pot)
        delete pot;
    }

    // testing information methods
    void testInformationMethods() {
      fill(*bn);

      gum::LazyPropagation<float> inf(*bn);
      inf.makeInference();

      TS_GUM_ASSERT_THROWS_NOTHING(inf.H((gum::NodeId)2));
      TS_GUM_ASSERT_THROWS_NOTHING(inf.I((gum::NodeId)2, (gum::NodeId)4));
      TS_GUM_ASSERT_THROWS_NOTHING(inf.I((gum::NodeId)2, (gum::NodeId)2));
      TS_GUM_ASSERT_THROWS_NOTHING(inf.VI((gum::NodeId)2, (gum::NodeId)4));
      TS_ASSERT_THROWS(inf.I((gum::NodeId)0, (gum::NodeId)4),
                       gum::OperationNotAllowed);

      //@TODO : test computations and not only good behaviour
    }

    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet<float> &bn) {
      const gum::Potential<float> &p1 = bn.cpt(i1);
      {
        // FILLING PARAMS
        const float t[2] = {0.2, 0.8};
        int n = 2;
        const std::vector<float> v(t, t + n);
        p1.fillWith(v);
      }

      const gum::Potential<float> &p2 = bn.cpt(i2);
      {
        // FILLING PARAMS
        const float t[2] = {0.3, 0.7};
        int n = 2;
        const std::vector<float> v(t, t + n);
        p2.fillWith(v);
      }

      const gum::Potential<float> &p3 = bn.cpt(i3);
      {
        // FILLING PARAMS
        const float t[4] = {0.1, 0.9, 0.9, 0.1};
        int n = 4;
        const std::vector<float> v(t, t + n);
        p3.fillWith(v);
      }

      const gum::Potential<float> &p4 = bn.cpt(i4);
      {
        // FILLING PARAMS
        const float t[8] = {0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0};
        int n = 8;
        const std::vector<float> v(t, t + n);
        p4.fillWith(v);
      }

      const gum::Potential<float> &p5 = bn.cpt(i5);
      {
        // FILLING PARAMS
        const float t[24] = {0.3, 0.6, 0.1, 0.5, 0.5, 0.0, 0.5, 0.5,
                             0.0, 1.0, 0.0, 0.0, 0.4, 0.6, 0.0, 0.5,
                             0.5, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0};
        int n = 24;
        const std::vector<float> v(t, t + n);
        p5.fillWith(v);
        // printProba(p5);
      }
    }

    // Uncomment this to have some outputs.
    void printProba(const gum::Potential<float> &) {
      // gum::Instantiation inst(p);

      // for (inst.setFirst(); !inst.end(); ++inst)
      //{
      //  std::cerr << inst<<" : " <<p[inst] << std::endl;
      //}
      // std::cerr << std::endl;
    }
  };
}
