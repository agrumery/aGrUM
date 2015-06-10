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
#include <sstream>
#include <cstdlib>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/generator/simpleBayesNetGenerator.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class ShaferShenoyInferenceBNTestSuite : public CxxTest::TestSuite {
    public:
    gum::BayesNet<float> *bn;
    gum::Id i1, i2, i3, i4, i5;

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
    }

    void tearDown() { delete bn; }

    void testFill() {
      const gum::Potential<float> &p1 = bn->cpt(i1);
      TS_ASSERT(p1.nbrDim() == 1);

      {
        // FILLING PARAMS
        const float t[2] = {0.2, 0.8};
        int n = 2;
        const std::vector<float> v(t, t + n);
        p1.fillWith(v);
      }

      const gum::Potential<float> &p2 = bn->cpt(i2);
      TS_ASSERT(p2.nbrDim() == 1);

      {
        // FILLING PARAMS
        const float t[2] = {0.3, 0.7};
        int n = 2;
        const std::vector<float> v(t, t + n);
        p2.fillWith(v);
      }

      const gum::Potential<float> &p3 = bn->cpt(i3);
      TS_ASSERT(p3.nbrDim() == 2);
      {
        // FILLING PARAMS
        const float t[4] = {0.1, 0.9, 0.9, 0.1};
        int n = 4;
        const std::vector<float> v(t, t + n);
        p3.fillWith(v);

        // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
        gum::Potential<float> p(new gum::MultiDimArray<float>());
        p << bn->variable(i1);
        p.marginalize(p3);

        for (gum::Instantiation j(p); !j.end(); ++j)
          TS_ASSERT_DELTA(p[j], 1.0, 1e-5);
      }

      const gum::Potential<float> &p4 = bn->cpt(i4);
      TS_ASSERT(p4.nbrDim() == 3);
      {
        // FILLING PARAMS
        const float t[8] = {0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0};
        int n = 8;
        const std::vector<float> v(t, t + n);
        p4.fillWith(v);

        // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
        gum::Potential<float> p(new gum::MultiDimArray<float>());
        p << bn->variable(i1) << bn->variable(i2);
        p.marginalize(p4);

        for (gum::Instantiation j(p); !j.end(); ++j)
          TS_ASSERT_DELTA(p[j], 1.0, 1e-5);
      }

      const gum::Potential<float> &p5 = bn->cpt(i5);
      TS_ASSERT(p5.nbrDim() == 4);
      {
        // FILLING PARAMS
        const float t[24] = {0.3, 0.6, 0.1, 0.5, 0.5, 0.0, 0.5, 0.5,
                             0.0, 1.0, 0.0, 0.0, 0.4, 0.6, 0.0, 0.5,
                             0.5, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0};
        int n = 24;
        const std::vector<float> v(t, t + n);
        p5.fillWith(v);

        // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
        gum::Potential<float> p(new gum::MultiDimArray<float>());
        p << bn->variable(i4) << bn->variable(i2) << bn->variable(i3);
        p.marginalize(p5);

        for (gum::Instantiation j(p); !j.end(); ++j) {
          TS_ASSERT_DELTA(p[j], 1.0, 1e-5);
        }
      }
    }

    // Testing when there is no evidence
    void testCreationAndInference() {
      try {
        fill(*bn);
        // Testing the inference
        gum::ShaferShenoyInference<float> *inf = 0;
        TS_ASSERT_THROWS_NOTHING(inf = new gum::ShaferShenoyInference<float>(*bn));
        TS_ASSERT_THROWS_NOTHING(inf->makeInference());

        if (inf != 0) {
          TS_ASSERT_THROWS_NOTHING(delete inf);
        }
      } catch (gum::Exception e) {
        TS_ASSERT(false);
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    void testMarginal() {
      fill(*bn);
      gum::ShaferShenoyInference<float> inf(*bn);

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
      e_list.insert(&(bn->cpt(i1)));
      e_list.insert(&(bn->cpt(i2)));

      gum::ShaferShenoyInference<float> inf(*bn);

      TS_ASSERT_THROWS_NOTHING(inf.insertEvidence(e_list));

      TS_ASSERT_THROWS_NOTHING(inf.makeInference());

      TS_ASSERT_THROWS_NOTHING(inf.posterior(i1));
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i2));
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i3));
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i4));
      TS_ASSERT_THROWS_NOTHING(inf.posterior(i5));
    }

    void testWithGenerator() {
      gum::Size density[] = {9, 18, 27, 36, 45};
      int trial_nb = 5;

      for (int i = 0; i < trial_nb; ++i) {
        gum::SimpleBayesNetGenerator<float> bnGen(10, density[i]);

        gum::BayesNet<float> *bayesNet = new gum::BayesNet<float>();
        bnGen.generateBN(*bayesNet);

        gum::ShaferShenoyInference<float> *inf = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(
            inf = new gum::ShaferShenoyInference<float>(*bayesNet));

        TS_GUM_ASSERT_THROWS_NOTHING(if (inf) inf->makeInference());

        TS_GUM_ASSERT_THROWS_NOTHING(if (inf) delete inf);

        delete bayesNet;
      }
    }

    private:
    // Builds a BN to tst the inference
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
      // for (inst.setFirst(); !inst.end(); ++inst)
      //{
      //  std::cerr << inst<<" : " <<p[inst] << std::endl;
      //}
      // std::cerr << std::endl;
    }
  };
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
