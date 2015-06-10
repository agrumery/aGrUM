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
#include <agrum/multidim/ICIModels/multiDimNoisyORNet.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/GibbsInference.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>

namespace gum_tests {

  class MultiDimNoisyORNetTestSuite : public CxxTest::TestSuite {
    public:
    void testCreationNoisyOr() {
      gum::LabelizedVariable a("a", "", 2), b("b", "", 2), c("c", "", 2),
          d("d", "", 2);
      gum::MultiDimNoisyORNet<float> p(0.2);

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

      TS_ASSERT_EQUALS(p.toString(),
                       "a<0,1>=noisyORNet([0.2],b<0,1>[0.4]c<0,1>[1]d<0,1>[0.7])");
      TS_ASSERT_EQUALS(p.realSize(), (gum::Size)4);

      gum::MultiDimNoisyORNet<float> q(p);
      TS_ASSERT_EQUALS(q.toString(),
                       "a<0,1>=noisyORNet([0.2],b<0,1>[0.4]c<0,1>[1]d<0,1>[0.7])");
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

    void testComputationInNoisyORNet() {
      gum::LabelizedVariable cold("Cold", "", 2);
      gum::LabelizedVariable flu("Flu", "", 2);
      gum::LabelizedVariable malaria("Malaria", "", 2);
      gum::LabelizedVariable fever("Fever", "", 2);

      gum::MultiDimNoisyORNet<float> p(0.0);
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

      gum::MultiDimNoisyORNet<float> q(p);

      j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(q[i], witness[j], 1e-6);
      }
    }

    void testComputationInNoisyORNet2() {
      gum::LabelizedVariable lazy("lazy", "", 2);
      gum::LabelizedVariable degree("degree", "", 2);
      gum::LabelizedVariable motivation("motivation", "", 2);
      gum::LabelizedVariable requirement("requirement", "", 2);
      gum::LabelizedVariable competition("competition", "", 2);
      gum::LabelizedVariable unemployment("unemployment", "", 2);

      gum::MultiDimNoisyORNet<float> p(0.0001);
      p << unemployment << competition << requirement << motivation << degree
        << lazy;
      p.causalWeight(lazy, 0.1);
      p.causalWeight(degree, 0.3);
      p.causalWeight(motivation, 0.5);
      p.causalWeight(requirement, 0.7);
      p.causalWeight(competition, 0.9);

      gum::Instantiation i(p);
      float witness[] = {0.9999,   0.0001,   0.09999,  0.90001,  0.29997,  0.70003,
                         0.029997, 0.970003, 0.49995,  0.50005,  0.049995, 0.950005,
                         0.149985, 0.850015, 0.014999, 0.985002, 0.69993,  0.30007,
                         0.069993, 0.930007, 0.209979, 0.790021, 0.020998, 0.979002,
                         0.349965, 0.650035, 0.034997, 0.965004, 0.104990, 0.895011,
                         0.010499, 0.989501, 0.89991,  0.10009,  0.089991, 0.910009,
                         0.269973, 0.730027, 0.026997, 0.973003, 0.449955, 0.550045,
                         0.044996, 0.955005, 0.134987, 0.865014, 0.013499, 0.986501,
                         0.629937, 0.370063, 0.062994, 0.937006, 0.188981, 0.811019,
                         0.018898, 0.981101, 0.314969, 0.685032, 0.031497, 0.968503,
                         0.094491, 0.905509, 0.009449, 0.990551};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6);
      }

      gum::MultiDimNoisyORNet<float> q(p);

      j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(q[i], witness[j], 1e-6);
      }
    }

    void testNoisyORNetInBN() {
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
      gum::NodeId idFever = bn.addNoisyORNet(fever, 0.0);
      gum::NodeId idOneMore = bn.add(oneMore);
      gum::NodeId idOneMoreParent1 = bn.add(oneMoreParent1);
      gum::NodeId idOneMoreParent2 = bn.add(oneMoreParent2);

      bn.addWeightedArc(idMalaria, idFever, 0.9);
      bn.addWeightedArc(idFlu, idFever, 0.8);
      bn.addWeightedArc(idCold, idFever, 0.4);

      TS_ASSERT_THROWS(bn.addWeightedArc(idMalaria, idCold, 0.8), gum::InvalidArc);

      const gum::Potential<float> &pOneMoreParent1 = bn.cpt(idOneMoreParent1);
      pOneMoreParent1.fillWith(std::vector<float>{0.2, 0.8});

      const gum::Potential<float> &pOneMoreParent2 = bn.cpt(idOneMoreParent2);
      pOneMoreParent2.fillWith(std::vector<float>{0.3, 0.7});

      bn.addArc(idOneMoreParent1, idOneMore);
      bn.addArc(idFever, idOneMore);
      bn.addArc(idOneMoreParent2, idOneMore);
      const gum::Potential<float> &pOneMore = bn.cpt(idOneMore);
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
