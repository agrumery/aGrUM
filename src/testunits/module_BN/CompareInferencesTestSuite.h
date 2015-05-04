/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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

#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/GibbsInference.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/io/BIF/BIFReader.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class CompareInferencesTestSuite : public CxxTest::TestSuite {
    private:
    // Builds a BN to test the inference
    void fill_bn1(gum::BayesNet<double> &bn) {
      gum::LabelizedVariable n1("1", "", 2), n2("2", "", 2), n3("3", "", 2);
      gum::LabelizedVariable n4("4", "", 2), n5("5", "", 3);

      i1 = bn.add(n1);
      i2 = bn.add(n2);
      i3 = bn.add(n3);
      i4 = bn.add(n4);
      i5 = bn.add(n5);

      bn.addArc(i1, i3);
      bn.addArc(i1, i4);
      bn.addArc(i3, i5);
      bn.addArc(i4, i5);
      bn.addArc(i2, i4);
      bn.addArc(i2, i5);

      const gum::Potential<double> &p1 = bn.cpt(i1);
      {
        const double t[2] = {0.2, 0.8};
        int n = 2;
        const std::vector<double> v(t, t + n);
        p1.fillWith(v);
      }

      const gum::Potential<double> &p2 = bn.cpt(i2);
      {
        const double t[2] = {0.3, 0.7};
        int n = 2;
        const std::vector<double> v(t, t + n);
        p2.fillWith(v);
      }

      const gum::Potential<double> &p3 = bn.cpt(i3);
      {
        const double t[4] = {0.1, 0.9, 0.9, 0.1};
        int n = 4;
        const std::vector<double> v(t, t + n);
        p3.fillWith(v);
      }

      const gum::Potential<double> &p4 = bn.cpt(i4);
      {
        const double t[8] = {0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 0.75, 0.25};
        int n = 8;
        const std::vector<double> v(t, t + n);
        p4.fillWith(v);
      }

      const gum::Potential<double> &p5 = bn.cpt(i5);
      {
        //         int n = 16;
        //
        //         const double t[16] = {0.4, 0.6,
        //                               0.5, 0.5,
        //                               0.5, 0.5,
        //                               1.0, 0.0,
        //                               0.4, 0.6,
        //                               0.5, 0.5,
        //                               0.5, 0.5,
        //                               0.0, 1.0,
        //                              };
        int n = 24;
        const double t[24] = {0.3,  0.6,  0.1,  0.5,  0.4,  0.1, 0.4, 0.5,
                              0.1,  0.9,  0.05, 0.05, 0.4,  0.5, 0.1, 0.5,
                              0.45, 0.05, 0.45, 0.5,  0.05, 0.1, 0.1, 0.8};
        const std::vector<double> v(t, t + n);
        p5.fillWith(v);
      }
    }

    void fill_bn2(gum::BayesNet<double> &bn) {
      gum::LabelizedVariable n1("1", "", 3), n2("2", "", 4), n3("3", "", 3);

      i1 = bn.add(n1);
      i5 = i2 = bn.add(n2);

      bn.addArc(i1, i2);

      const gum::Potential<double> &p1 = bn.cpt(i1);
      {
        const double t[3] = {0.2, 0.7, 0.1};
        int n = 3;
        const std::vector<double> v(t, t + n);
        p1.fillWith(v);
      }

      const gum::Potential<double> &p5 = bn.cpt(i5);
      {
        int n = 12;
        const double t[12] = {0.1, 0.2, 0.3, 0.4, 0.3, 0.2,
                              0.2, 0.3, 0.4, 0.3, 0.2, 0.1};
        /*
        const double t[n] = {0.3, 0.6, 0.1, 0.5, 0.5,0.0, 0.5, 0.5,0.0, 1.0, 0.0,
        0.0,
        0.4, 0.6, 0.0,0.5, 0.5,0.0, 0.5, 0.5,0.0, 0.0, 0.0,1.0
        };*/
        const std::vector<double> v(t, t + n);
        p5.fillWith(v);
      }
    }

    public:
    gum::BayesNet<double> *bn;
    gum::NodeId i1, i2, i3, i4, i5;

    void setUp() {
      bn = new gum::BayesNet<double>();

      fill_bn1(*bn);
    }

    void tearDown() { delete (bn); }

    void testInferencesWithNoEvidence() {
      gum::ShaferShenoyInference<double> inf_ShaShe(*bn);
      inf_ShaShe.makeInference();

      gum::LazyPropagation<double> inf_LazyProp(*bn);
      inf_LazyProp.makeInference();

      gum::VariableElimination<double> inf_ValElim(*bn);
      inf_ValElim.makeInference();

      gum::GibbsInference<double> inf_gibbs(*bn);
      inf_gibbs.setVerbosity(false);
      inf_gibbs.setEpsilon(1e-5);
      inf_gibbs.setMinEpsilonRate(1e-5);
      TS_GUM_ASSERT_THROWS_NOTHING(inf_gibbs.makeInference());

      for (const auto i : bn->nodes()) {
        const gum::Potential<double> &marginal_gibbs = inf_gibbs.posterior(i);
        const gum::Potential<double> &marginal_ShaShe = inf_ShaShe.posterior(i);
        const gum::Potential<double> &marginal_LazyProp = inf_LazyProp.posterior(i);
        const gum::Potential<double> &marginal_ValElim = inf_ValElim.posterior(i);

        gum::Instantiation I;
        I << bn->variable(i);

        for (I.setFirst(); !I.end(); ++I) {
          TS_ASSERT_DELTA(marginal_gibbs[I], marginal_ShaShe[I],
                          5e-3); // APPROX INFERENCE
          TS_ASSERT_DELTA(marginal_LazyProp[I], marginal_ShaShe[I],
                          1e-10); // EXACT INFERENCE
          TS_ASSERT_DELTA(marginal_LazyProp[I], marginal_ValElim[I],
                          1e-10); // EXACT INFERENCE
          TS_ASSERT_DELTA(marginal_ShaShe[I], marginal_ValElim[I],
                          1e-10); // EXACT INFERENCE
        }
      }
    }

    void testMultipleInference() {
      gum::BayesNet<float> *bn;
      gum::Id c, s, r, w;

      bn = new gum::BayesNet<float>();

      gum::LabelizedVariable vc("c", "cloudy", 2), vs("s", "sprinklet", 2);
      gum::LabelizedVariable vr("r", "rain", 2), vw("w", "wet grass", 2);

      c = bn->add(vc);
      s = bn->add(vs);
      r = bn->add(vr);
      w = bn->add(vw);

      bn->addArc(c, s);
      bn->addArc(c, r);
      bn->addArc(s, w);
      bn->addArc(r, w);

      gum::Potential<float> *e_i1, *e_i4;
      e_i1 = new gum::Potential<float>();
      (*e_i1) << bn->variable(c);
      e_i1->fill((float)0);
      gum::Instantiation inst_1(*e_i1);
      inst_1.chgVal(bn->variable(c), 0);
      e_i1->set(inst_1, (float)1);

      e_i4 = new gum::Potential<float>();
      (*e_i4) << bn->variable(s);
      e_i4->fill((float)0);
      gum::Instantiation inst_4(*e_i4);
      inst_4.chgVal(bn->variable(s), 1);
      e_i4->set(inst_4, (float)1);

      {
        const float t[2] = {0.5, 0.5};
        const std::vector<float> ca(t, t + 2);
        bn->cpt(c).fillWith(ca);
      }

      {
        const float t[4] = {0.5, 0.5, 0.9, 0.1};
        const std::vector<float> sa(t, t + 4);
        bn->cpt(s).fillWith(sa);
      }

      {
        const float t[4] = {0.8, 0.2, 0.2, 0.8};
        const std::vector<float> ra(t, t + 4);
        bn->cpt(r).fillWith(ra);
      }

      {
        const float t[8] = {1., 0., 0.1, 0.9, 0.1, 0.9, 0.01, 0.99};
        const std::vector<float> wa(t, t + 8);
        bn->cpt(w).fillWith(wa);
      }

      gum::List<gum::Potential<float> const *> list_pot =
          gum::List<gum::Potential<float> const *>();
      {
        gum::Potential<float> *pot = new gum::Potential<float>();
        pot->add(bn->variable(s));
        const float t[2] = {0., 1.};
        const std::vector<float> sa(t, t + 2);
        pot->fillWith(sa);
        list_pot.insert(pot);
      }

      {
        gum::Potential<float> *pot = new gum::Potential<float>();
        pot->add(bn->variable(c));
        const float t[2] = {1., 0.};
        const std::vector<float> ca(t, t + 2);
        pot->fillWith(ca);
        list_pot.insert(pot);
      }

      {
        gum::GibbsInference<float> inf(*bn);
        inf.setVerbosity(false);
        inf.makeInference();
        {
          const gum::Potential<float> &p = inf.posterior(w);
          gum::Instantiation I(p);
          TS_ASSERT_DELTA(p[I], 0.3529, 3e-2);
          ++I;
          TS_ASSERT_DELTA(p[I], 0.6471, 3e-2);
        }

        inf.eraseAllEvidence();
        inf.insertEvidence(list_pot);
        inf.makeInference();
        {
          const gum::Potential<float> &p = inf.posterior(w);
          gum::Instantiation I(p);
          TS_ASSERT_DELTA(p[I], 0.082, 1e-2);
          ++I;
          TS_ASSERT_DELTA(p[I], 0.918, 1e-2);
        }
      }

      {
        gum::LazyPropagation<float> inf(*bn);
        inf.makeInference();
        {
          const gum::Potential<float> &p = inf.posterior(w);
          gum::Instantiation I(p);
          TS_ASSERT_DELTA(p[I], 0.3529, 1e-7);
          ++I;
          TS_ASSERT_DELTA(p[I], 0.6471, 1e-7);
        }

        inf.eraseAllEvidence();
        inf.insertEvidence(list_pot);
        inf.makeInference();
        {
          const gum::Potential<float> &p = inf.posterior(w);
          gum::Instantiation I(p);
          TS_ASSERT_DELTA(p[I], 0.082, 1e-7);
          ++I;
          TS_ASSERT_DELTA(p[I], 0.918, 1e-7);
        }
      }

      {
        gum::ShaferShenoyInference<float> inf(*bn);
        inf.makeInference();
        {
          const gum::Potential<float> &p = inf.posterior(w);
          gum::Instantiation I(p);
          TS_ASSERT_DELTA(p[I], 0.3529, 1e-5);
          ++I;
          TS_ASSERT_DELTA(p[I], 0.6471, 1e-5);
        }

        inf.eraseAllEvidence();
        inf.insertEvidence(list_pot);
        inf.makeInference();
        {
          const gum::Potential<float> &p = inf.posterior(w);
          gum::Instantiation I(p);
          TS_ASSERT_DELTA(p[I], 0.082, 1e-7);
          ++I;
          TS_ASSERT_DELTA(p[I], 0.918, 1e-7);
        }
      }

      {
        gum::VariableElimination<float> inf(*bn);
        inf.makeInference();
        {
          const gum::Potential<float> &p = inf.posterior(w);
          gum::Instantiation I(p);
          TS_ASSERT_DELTA(p[I], 0.3529, 1e-5);
          ++I;
          TS_ASSERT_DELTA(p[I], 0.6471, 1e-5);
        }

        inf.eraseAllEvidence();
        inf.insertEvidence(list_pot);
        inf.makeInference();
        {
          const gum::Potential<float> &p = inf.posterior(w);
          gum::Instantiation I(p);
          TS_ASSERT_DELTA(p[I], 0.082, 1e-7);
          ++I;
          TS_ASSERT_DELTA(p[I], 0.918, 1e-7);
        }
      }

      for (const auto pot : list_pot)
        delete pot;

      delete e_i1;
      delete e_i4;
      delete bn;
    }

    // compare only Lazy and ShaferShenoy on alarm BN
    void testAlarmInference() {

      std::string file = GET_PATH_STR("alarm.bif");

      gum::BayesNet<float> *net = new gum::BayesNet<float>();

      gum::BIFReader<float> reader(net, file);

      int nbrErr = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());

      TS_ASSERT(nbrErr == 0);

      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      {
        gum::LazyPropagation<float> infLazy(*net);
        infLazy.makeInference();

        gum::LazyPropagation<float> infShaf(*net);
        infShaf.makeInference();

        for (const auto node : net->nodes()) {
          gum::Instantiation I;
          I << net->variable(node);

          for (I.setFirst(); !I.end(); ++I) {
            TS_ASSERT_DELTA(infLazy.posterior(node)[I], infShaf.posterior(node)[I],
                            1e-6);
          }
        }
      }

      delete net;
    }
  };
}
