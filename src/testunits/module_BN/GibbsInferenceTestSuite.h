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

#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/GibbsInference.h>

#include <agrum/core/algorithms/approximationScheme/approximationSchemeListener.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class aSimpleGibbsListener : public gum::ApproximationSchemeListener {
    private:
    int __nbr;
    std::string __mess;

    public:
    aSimpleGibbsListener(gum::ApproximationScheme &sch)
        : gum::ApproximationSchemeListener(sch), __nbr(0), __mess(""){};
    void whenProgress(const void *buffer, gum::Size a, double b, double c) {
      __nbr++;
    }
    void whenStop(const void *buffer, std::string s) { __mess = s; }

    int getNbr() { return __nbr; }
    std::string getMess() { return __mess; }
  };

  class GibbsInferenceTestSuite : public CxxTest::TestSuite {
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
    void testGibbsInf_1() {
      //       try {
      //         fill( *bn );
      //         // Testing the inference
      //         gum::GibbsInference<float> inf( *bn );
      //         inf.setVerbosity( false );
      //         inf.makeInference();
      //       } catch ( gum::Exception e ) {
      //         TS_ASSERT( false );
      //         std::cerr << std::endl << e.content() << std::endl;
      //         throw e;
      //       }
    }

    void testGibbsInf_2() {
      fill(*bn);
      gum::GibbsInference<float> inf(*bn);
      inf.setVerbosity(false);

      try {
        // Testing the inference
        inf.makeInference();
      } catch (gum::Exception e) {
        TS_ASSERT(false);
      }

      try {
        const gum::Potential<float> &posterior = inf.posterior(i1);
        printPotential(posterior);
      } catch (gum::Exception e) {
        TS_ASSERT(false);
      }

      try {
        const gum::Potential<float> &posterior = inf.posterior(i2);
        printPotential(posterior);
      } catch (gum::Exception e) {
        std::cerr << e.errorContent() << std::endl;
        TS_ASSERT(false);
      }

      try {
        const gum::Potential<float> &posterior = inf.posterior(i3);
        printPotential(posterior);
      } catch (gum::Exception e) {
        TS_ASSERT(false);
      }

      try {
        const gum::Potential<float> &posterior = inf.posterior(i4);
        printPotential(posterior);
      } catch (gum::Exception e) {
        TS_ASSERT(false);
      }

      try {
        const gum::Potential<float> &posterior = inf.posterior(i5);
        printPotential(posterior);
      } catch (gum::Exception e) {
        TS_ASSERT(false);
      }
    }

    void testGibbsInf_3() {
      fill(*bn);
      gum::List<const gum::Potential<float> *> e_list;
      e_list.insert(e_i1);
      e_list.insert(e_i4);

      gum::GibbsInference<float> inf(*bn);
      inf.setVerbosity(false);

      try {
        inf.insertEvidence(e_list);
      } catch (gum::Exception e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        TS_ASSERT(false);
      }

      try {
        // Testing the inference
        inf.makeInference();
      } catch (gum::Exception e) {
        TS_ASSERT(false);
      }

      try {
        const gum::Potential<float> &posterior = inf.posterior(i1);
        printPotential(posterior);
      } catch (gum::Exception e) {
        TS_ASSERT(false);
      }

      try {
        const gum::Potential<float> &posterior = inf.posterior(i2);
        printPotential(posterior);
      } catch (gum::Exception e) {
        TS_ASSERT(false);
      }

      try {
        const gum::Potential<float> &posterior = inf.posterior(i3);
        printPotential(posterior);
      } catch (gum::Exception e) {
        TS_ASSERT(false);
      }

      try {
        const gum::Potential<float> &posterior = inf.posterior(i4);
        printPotential(posterior);
      } catch (gum::Exception e) {
        TS_ASSERT(false);
      }

      try {
        const gum::Potential<float> &posterior = inf.posterior(i5);
        printPotential(posterior);
      } catch (gum::Exception e) {
        TS_ASSERT(false);
      }
    }

    void testGibbsInfListener() {
      fill(*bn);
      gum::List<const gum::Potential<float> *> e_list;
      e_list.insert(e_i1);
      e_list.insert(e_i4);

      gum::GibbsInference<float> inf(*bn);

      aSimpleGibbsListener agsl(inf);

      try {
        // Testing the inference
        inf.makeInference();
      } catch (gum::Exception e) {
        TS_ASSERT(false);
      }

      TS_ASSERT_EQUALS(agsl.getNbr() * inf.periodSize() + inf.burnIn(),
                       inf.nbrIterations());
      TS_ASSERT_DIFFERS(agsl.getMess(), std::string(""));
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
        // printPotential(p5);
      }
    }

    // Uncomment this to have some outputs.
    void printPotential(const gum::Potential<float> &) {
      // gum::Instantiation inst(p);

      // for (inst.setFirst(); !inst.end(); ++inst)
      //{
      //  std::cerr << inst<<" : " <<p[inst] << std::endl;
      //}
      // std::cerr << std::endl;
    }
  };
}
