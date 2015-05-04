/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <sstream>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/variables/rangeVariable.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/GibbsInference.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>

namespace gum_tests {

  class MultiDimNoisyORCompoundTestSuite : public CxxTest::TestSuite {
    public:
    void testCreationNoisyOr() {
      gum::LabelizedVariable a("a", "", 2), b("b", "", 2), c("c", "", 2),
          d("d", "", 2);
      gum::MultiDimNoisyORCompound<float> p(0.2);

      // trying to change weight for a non cause
      TS_ASSERT_THROWS(p.causalWeight(b, 0.4), gum::InvalidArgument);
      TS_ASSERT_THROWS(p.causalWeight(d, 0.0), gum::InvalidArgument);

      // adding causes
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c << d);

      // trying to set 0 for causal weight
      TS_ASSERT_THROWS(p.causalWeight(d, 0.0), gum::OutOfBounds);

      // doing the right stuff :)
      p.causalWeight(b, 0.4);
      p.causalWeight(d, 0.7);
      TS_ASSERT_EQUALS(
          p.toString(),
          "a<0,1>=noisyORCompound([0.2],b<0,1>[0.4]c<0,1>[1]d<0,1>[0.7])");
      TS_ASSERT_EQUALS(p.realSize(), (gum::Size)4);

      gum::MultiDimNoisyORCompound<float> q(p);
      TS_ASSERT_EQUALS(
          q.toString(),
          "a<0,1>=noisyORCompound([0.2],b<0,1>[0.4]c<0,1>[1]d<0,1>[0.7])");
      TS_ASSERT_EQUALS(p.realSize(), (gum::Size)4);
    }

    void testCompatibleWithHardOR() {
      gum::LabelizedVariable cold("Cold", "", 2);
      gum::LabelizedVariable flu("Flu", "", 2);
      gum::LabelizedVariable malaria("Malaria", "", 2);
      gum::LabelizedVariable fever("Fever", "", 2);

      gum::MultiDimNoisyORCompound<float> p(0.0);
      p << fever << malaria << flu << cold;
      p.causalWeight(cold, 1.0);
      p.causalWeight(flu, 1.0);
      p.causalWeight(malaria, 1.0);

      gum::Instantiation i(p);
      float witness[] = {1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0,
                         0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6);
      }
    }

    void testComputationInNoisyORCompound() {
      gum::LabelizedVariable cold("Cold", "", 2);
      gum::LabelizedVariable flu("Flu", "", 2);
      gum::LabelizedVariable malaria("Malaria", "", 2);
      gum::LabelizedVariable fever("Fever", "", 2);

      gum::MultiDimNoisyORCompound<float> p(0.0);
      p << fever << malaria << flu << cold;
      p.causalWeight(cold, 0.4);
      p.causalWeight(flu, 0.8);
      p.causalWeight(malaria, 0.9);

      gum::Instantiation i(p);
      float witness[] = {1,   0,   0.1,  0.9,  0.2,  0.8,  0.02,  0.98,
                         0.6, 0.4, 0.06, 0.94, 0.12, 0.88, 0.012, 0.988};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6);
      }

      gum::MultiDimNoisyORCompound<float> q(p);

      j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(q[i], witness[j], 1e-6);
      }
    }

    void testComputationInNoisyORCompound2() {
      gum::LabelizedVariable lazy("lazy", "", 2);
      gum::LabelizedVariable degree("degree", "", 2);
      gum::LabelizedVariable motivation("motivation", "", 2);
      gum::LabelizedVariable requirement("requirement", "", 2);
      gum::LabelizedVariable competition("competition", "", 2);
      gum::LabelizedVariable unemployment("unemployment", "", 2);

      gum::MultiDimNoisyORCompound<float> p(0.0001);
      p << unemployment << competition << requirement << motivation << degree
        << lazy;
      p.causalWeight(lazy, 0.1);
      p.causalWeight(degree, 0.3);
      p.causalWeight(motivation, 0.5);
      p.causalWeight(requirement, 0.7);
      p.causalWeight(competition, 0.9);

      gum::Instantiation i(p);
      float witness[] = {0.9999,   0.0001,   0.1,      0.9,      0.3,      0.7,
                         0.030003, 0.969997, 0.5,      0.5,      0.050005, 0.949995,
                         0.150015, 0.849985, 0.015003, 0.984997, 0.7,      0.3,
                         0.070007, 0.929993, 0.210021, 0.789979, 0.021004, 0.978996,
                         0.350035, 0.649965, 0.035007, 0.964993, 0.105021, 0.894979,
                         0.010503, 0.989496, 0.9,      0.1,      0.090009, 0.909991,
                         0.270027, 0.729973, 0.027005, 0.972995, 0.450045, 0.549955,
                         0.045009, 0.954991, 0.135027, 0.864973, 0.013504, 0.986496,
                         0.630063, 0.369937, 0.063013, 0.936987, 0.189038, 0.810962,
                         0.018906, 0.981094, 0.315063, 0.684937, 0.031509, 0.968491,
                         0.094528, 0.905472, 0.009454, 0.990546};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6);
      }

      gum::MultiDimNoisyORCompound<float> q(p);

      j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(q[i], witness[j], 1e-6);
      }
    }

    void testNoisyORCompoundInBN() {
      gum::BayesNet<float> bn;

      gum::LabelizedVariable cold("Cold", "", 2);
      gum::LabelizedVariable flu("Flu", "", 2);
      gum::LabelizedVariable malaria("Malaria", "", 2);
      gum::LabelizedVariable fever("Fever", "", 2);
      gum::LabelizedVariable oneMore("OneMore", "", 2);
      gum::LabelizedVariable oneMoreParent1("OneMoreParent1", "", 2);
      gum::LabelizedVariable oneMoreParent2("OneMoreParent2", "", 2);

      gum::NodeId idCold = bn.add(cold);
      gum::NodeId idFlu = bn.add(flu);
      gum::NodeId idMalaria = bn.add(malaria);
      gum::NodeId idFever = bn.addNoisyOR(fever, 0.0);
      gum::NodeId idOneMore = bn.add(oneMore);
      gum::NodeId idOneMoreParent1 = bn.add(oneMoreParent1);
      gum::NodeId idOneMoreParent2 = bn.add(oneMoreParent2);

      bn.addWeightedArc(idMalaria, idFever, 0.9);
      bn.addWeightedArc(idFlu, idFever, 0.8);
      bn.addWeightedArc(idCold, idFever, 0.4);

      TS_ASSERT_THROWS(bn.addWeightedArc(idMalaria, idCold, 0.8), gum::InvalidArc);

      const gum::Potential<float> &pOneMoreParent1 = bn.cpt(idOneMoreParent1);
      // FILLING PARAMS
      pOneMoreParent1.fillWith(std::vector<float>{0.2, 0.8});

      const gum::Potential<float> &pOneMoreParent2 = bn.cpt(idOneMoreParent2);
      // FILLING PARAMS
      pOneMoreParent2.fillWith(std::vector<float>{0.3, 0.7});

      bn.addArc(idOneMoreParent1, idOneMore);
      bn.addArc(idFever, idOneMore);
      bn.addArc(idOneMoreParent2, idOneMore);
      const gum::Potential<float> &pOneMore = bn.cpt(idOneMore);
      // FILLING PARAMS
      pOneMore.fillWith(std::vector<float>{0.1, 0.9, 0.8, 0.2, 0.1, 0.9, 0.8, 0.2,
                                           0.1, 0.9, 0.8, 0.2, 0.1, 0.9, 0.8, 0.2});

      const gum::Potential<float> &p = bn.cpt(idFever);

      gum::Instantiation i(p);
      float witness[] = {1.0, 0.0, 0.1,  0.9,  0.2,  0.8,  0.02,  0.98,
                         0.6, 0.4, 0.06, 0.94, 0.12, 0.88, 0.012, 0.988};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6);
      }

      gum::LazyPropagation<float> inf_LazyProp(bn);

      inf_LazyProp.makeInference();
    }
  };
}
