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


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/multidim/potential.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/loopyBeliefPropagation.h>
#include <agrum/tools/core/exceptions.h>
#include <agrum/tools/multidim/ICIModels/multiDimNoisyAND.h>
#include <agrum/tools/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/tools/multidim/ICIModels/multiDimNoisyORNet.h>
#include <agrum/tools/multidim/aggregators/amplitude.h>
#include <agrum/tools/multidim/aggregators/and.h>
#include <agrum/tools/multidim/aggregators/count.h>
#include <agrum/tools/multidim/aggregators/exists.h>
#include <agrum/tools/multidim/aggregators/forall.h>
#include <agrum/tools/multidim/aggregators/max.h>
#include <agrum/tools/multidim/aggregators/median.h>
#include <agrum/tools/multidim/aggregators/min.h>
#include <agrum/tools/multidim/aggregators/or.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/tools/variables/rangeVariable.h>

namespace gum_tests {

  class MultiDimAggregratorsForBNTestSuite: public CxxTest::TestSuite {
    public:
    void testBNwithMin() {
      gum::List< gum::NodeId > idList;
      gum::BayesNet< double >  bn;

      int nbr = 6;   // nbr=nbr of parents

      std::string                 str;
      const double                t[4] = {0.1f, 0.1f, 0.1f, 0.7f};
      int                         n    = 4;
      const std::vector< double > v(t, t + n);

      for (int i = 0; i <= nbr; i++) {
        str = "";
        std::stringstream ss;
        ss << "var" << (i + 1);
        ss >> str;

        gum::LabelizedVariable var(str, str, 4);

        if (i == 0) {
          idList.insert(bn.add(var, new gum::aggregator::Min< double >()));
        } else {
          idList.insert(bn.add(var));
          bn.addArc(idList[i], idList[0]);
          bn.cpt(idList[i]).fillWith(v);

          gum::Instantiation i(bn.cpt(idList[0]));

          gum::Idx res;

          for (i.setFirst(); !i.end(); ++i) {
            res = (gum::Idx)10000;   // clearly arbitraty choosen

            for (gum::Idx j = 1; j < i.nbrDim(); j++) {
              if (res > i.val(j)) res = i.val(j);
            }

            TS_ASSERT_EQUALS(bn.cpt(idList[0])[i], (res == i.val((gum::Idx)0)) ? 1.0f : 0.0f);
          }
        }
      }

      {
        gum::ShaferShenoyInference< double > inf(&bn);

        try {
          // Testing the inference
          inf.makeInference();
        } catch (gum::Exception e) { TS_ASSERT(false); }

        try {
          TS_ASSERT_EQUALS(inf.posterior(idList[0]),
                           (gum::Potential< double >() << bn.variable(idList[0]))
                              .fillWith({0.468559, 0.269297, 0.144495, 0.117649}))
          TS_ASSERT_EQUALS(
             inf.posterior(idList[1]),
             (gum::Potential< double >() << bn.variable(idList[1])).fillWith({0.1, 0.1, 0.1, 0.7}))
        } catch (gum::Exception e) { TS_ASSERT(false); }
      }

      {
        gum::LazyPropagation< double > inf(&bn);

        try {
          // Testing the inference
          inf.makeInference();
        } catch (gum::Exception e) { TS_ASSERT(false); }

        try {
          TS_ASSERT_EQUALS(inf.posterior(idList[0]),
                           (gum::Potential< double >() << bn.variable(idList[0]))
                              .fillWith({0.468559, 0.269297, 0.144495, 0.117649}))
          TS_ASSERT_EQUALS(
             inf.posterior(idList[1]),
             (gum::Potential< double >() << bn.variable(idList[1])).fillWith({0.1, 0.1, 0.1, 0.7}))
        } catch (gum::Exception) { TS_ASSERT(false); }
      }
    }

    void testNoisyORNetInBN() {
      gum::BayesNet< double > bn;

      gum::LabelizedVariable cold("Cold", "", 2);
      gum::LabelizedVariable flu("Flu", "", 2);
      gum::LabelizedVariable malaria("Malaria", "", 2);
      gum::LabelizedVariable fever("Fever", "", 2);
      gum::LabelizedVariable oneMore("OneMore", "", 2);
      gum::LabelizedVariable oneMoreParent1("OneMoreParent1", "", 2);
      gum::LabelizedVariable oneMoreParent2("OneMoreParent2", "", 2);

      gum::NodeId idCold           = bn.add(cold);
      gum::NodeId idFlu            = bn.add(flu);
      gum::NodeId idMalaria        = bn.add(malaria);
      gum::NodeId idFever          = bn.addNoisyORNet(fever, 0.0f);
      gum::NodeId idOneMore        = bn.add(oneMore);
      gum::NodeId idOneMoreParent1 = bn.add(oneMoreParent1);
      gum::NodeId idOneMoreParent2 = bn.add(oneMoreParent2);

      bn.addWeightedArc(idMalaria, idFever, 0.9f);
      bn.addWeightedArc(idFlu, idFever, 0.8f);
      bn.addWeightedArc(idCold, idFever, 0.4f);

      TS_ASSERT_THROWS(bn.addWeightedArc(idMalaria, idCold, 0.8f), gum::InvalidArc);

      const gum::Potential< double >& pOneMoreParent1 = bn.cpt(idOneMoreParent1);
      pOneMoreParent1.fillWith(std::vector< double >{0.2f, 0.8f});

      const gum::Potential< double >& pOneMoreParent2 = bn.cpt(idOneMoreParent2);
      pOneMoreParent2.fillWith(std::vector< double >{0.3f, 0.7f});

      bn.addArc(idOneMoreParent1, idOneMore);
      bn.addArc(idFever, idOneMore);
      bn.addArc(idOneMoreParent2, idOneMore);
      const gum::Potential< double >& pOneMore = bn.cpt(idOneMore);
      pOneMore.fillWith(std::vector< double >{0.1f,
                                              0.9f,
                                              0.8f,
                                              0.2f,
                                              0.1f,
                                              0.9f,
                                              0.8f,
                                              0.2f,
                                              0.1f,
                                              0.9f,
                                              0.8f,
                                              0.2f,
                                              0.1f,
                                              0.9f,
                                              0.8f,
                                              0.2f});

      const gum::Potential< double >& p = bn.cpt(idFever);

      gum::Instantiation i(p);
      float              witness[] = {1.0f,
                         0.0f,
                         0.1f,
                         0.9f,
                         0.2f,
                         0.8f,
                         0.02f,
                         0.98f,
                         0.6f,
                         0.4f,
                         0.06f,
                         0.94f,
                         0.12f,
                         0.88f,
                         0.012f,
                         0.988f};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6);
      }

      gum::LazyPropagation< double > inf_LazyProp(&bn);

      inf_LazyProp.makeInference();
    }

    void testNoisyORCompoundInBN() {
      gum::BayesNet< double > bn;

      gum::LabelizedVariable cold("Cold", "", 2);
      gum::LabelizedVariable flu("Flu", "", 2);
      gum::LabelizedVariable malaria("Malaria", "", 2);
      gum::LabelizedVariable fever("Fever", "", 2);
      gum::LabelizedVariable oneMore("OneMore", "", 2);
      gum::LabelizedVariable oneMoreParent1("OneMoreParent1", "", 2);
      gum::LabelizedVariable oneMoreParent2("OneMoreParent2", "", 2);

      gum::NodeId idCold           = bn.add(cold);
      gum::NodeId idFlu            = bn.add(flu);
      gum::NodeId idMalaria        = bn.add(malaria);
      gum::NodeId idFever          = bn.addNoisyOR(fever, 0.0f);
      gum::NodeId idOneMore        = bn.add(oneMore);
      gum::NodeId idOneMoreParent1 = bn.add(oneMoreParent1);
      gum::NodeId idOneMoreParent2 = bn.add(oneMoreParent2);

      bn.addWeightedArc(idMalaria, idFever, 0.9f);
      bn.addWeightedArc(idFlu, idFever, 0.8f);
      bn.addWeightedArc(idCold, idFever, 0.4f);

      TS_ASSERT_THROWS(bn.addWeightedArc(idMalaria, idCold, 0.8f), gum::InvalidArc);

      const gum::Potential< double >& pOneMoreParent1 = bn.cpt(idOneMoreParent1);
      // FILLING PARAMS
      pOneMoreParent1.fillWith(std::vector< double >{0.2f, 0.8f});

      const gum::Potential< double >& pOneMoreParent2 = bn.cpt(idOneMoreParent2);
      // FILLING PARAMS
      pOneMoreParent2.fillWith(std::vector< double >{0.3f, 0.7f});

      bn.addArc(idOneMoreParent1, idOneMore);
      bn.addArc(idFever, idOneMore);
      bn.addArc(idOneMoreParent2, idOneMore);
      const gum::Potential< double >& pOneMore = bn.cpt(idOneMore);
      // FILLING PARAMS
      pOneMore.fillWith(std::vector< double >{0.1f,
                                              0.9f,
                                              0.8f,
                                              0.2f,
                                              0.1f,
                                              0.9f,
                                              0.8f,
                                              0.2f,
                                              0.1f,
                                              0.9f,
                                              0.8f,
                                              0.2f,
                                              0.1f,
                                              0.9f,
                                              0.8f,
                                              0.2f});

      const gum::Potential< double >& p = bn.cpt(idFever);

      gum::Instantiation i(p);
      float              witness[] = {1.0f,
                         0.0f,
                         0.1f,
                         0.9f,
                         0.2f,
                         0.8f,
                         0.02f,
                         0.98f,
                         0.6f,
                         0.4f,
                         0.06f,
                         0.94f,
                         0.12f,
                         0.88f,
                         0.012f,
                         0.988f};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6);
      }

      gum::LazyPropagation< double > inf_LazyProp(&bn);

      inf_LazyProp.makeInference();
    }

    void testNoisyANDInBN() {
      gum::BayesNet< double > bn;

      gum::LabelizedVariable cold("Cold", "", 2);
      gum::LabelizedVariable flu("Flu", "", 2);
      gum::LabelizedVariable malaria("Malaria", "", 2);
      gum::LabelizedVariable fever("Fever", "", 2);
      gum::LabelizedVariable oneMore("OneMore", "", 2);
      gum::LabelizedVariable oneMoreParent1("OneMoreParent1", "", 2);
      gum::LabelizedVariable oneMoreParent2("OneMoreParent2", "", 2);

      gum::NodeId idCold    = bn.add(cold);
      gum::NodeId idFlu     = bn.add(flu);
      gum::NodeId idMalaria = bn.add(malaria);
      gum::NodeId idFever   = 0;
      TS_ASSERT_THROWS(idFever = bn.addNoisyAND(fever, 0.0f), gum::InvalidArgument);
      TS_GUM_ASSERT_THROWS_NOTHING(idFever = bn.addNoisyAND(fever, 0.999f));
      gum::NodeId idOneMore        = bn.add(oneMore);
      gum::NodeId idOneMoreParent1 = bn.add(oneMoreParent1);
      gum::NodeId idOneMoreParent2 = bn.add(oneMoreParent2);

      bn.addWeightedArc(idMalaria, idFever, 0.9f);
      bn.addWeightedArc(idFlu, idFever, 0.8f);
      bn.addWeightedArc(idCold, idFever, 0.4f);

      TS_ASSERT_THROWS(bn.addWeightedArc(idMalaria, idCold, 0.8f), gum::InvalidArc);

      const gum::Potential< double >& pOneMoreParent1 = bn.cpt(idOneMoreParent1);
      // FILLING PARAMS
      pOneMoreParent1.fillWith(std::vector< double >{0.2f, 0.8f});

      const gum::Potential< double >& pOneMoreParent2 = bn.cpt(idOneMoreParent2);
      // FILLING PARAMS
      pOneMoreParent2.fillWith(std::vector< double >{0.3f, 0.7f});

      bn.addArc(idOneMoreParent1, idOneMore);
      bn.addArc(idFever, idOneMore);
      bn.addArc(idOneMoreParent2, idOneMore);
      const gum::Potential< double >& pOneMore = bn.cpt(idOneMore);
      // FILLING PARAMS
      pOneMore.fillWith(std::vector< double >{0.1f,
                                              0.9f,
                                              0.8f,
                                              0.2f,
                                              0.1f,
                                              0.9f,
                                              0.8f,
                                              0.2f,
                                              0.1f,
                                              0.9f,
                                              0.8f,
                                              0.2f,
                                              0.1f,
                                              0.9f,
                                              0.8f,
                                              0.2f});

      const gum::Potential< double >& p = bn.cpt(idFever);

      gum::Instantiation i(p);
      float              witness[] = {0.988012f,
                         0.011988f,
                         0.892108f,
                         0.107892f,
                         0.952048f,
                         0.047952f,
                         0.568432f,
                         0.431568f,
                         0.992008f,
                         0.007992f,
                         0.928072f,
                         0.071928f,
                         0.968032f,
                         0.031968f,
                         0.712288f,
                         0.287712f};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6);
      }

      gum::LazyPropagation< double > inf_LazyProp(&bn);

      inf_LazyProp.makeInference();
    }

    void testBNwithMinNoParents() {
      auto                   bn = gum::BayesNet< double >::fastPrototype("A->B->C");
      gum::LabelizedVariable var("min", "min", 4);
      bn.add(var, new gum::aggregator::Min< double >());
      {
        gum::LazyPropagation< double > ie(&bn);
        ie.makeInference();
        gum::Potential< double > p;
        TS_GUM_ASSERT_THROWS_NOTHING(p = ie.posterior("min"));
        gum::Instantiation I(p);
        for (I.setFirst(); !I.end(); I.inc())
          TS_ASSERT_EQUALS(p.get(I), I.val(0) == 3 ? 1 : 0);
      }
      {
        gum::ShaferShenoyInference< double > ie(&bn);
        ie.makeInference();
        gum::Potential< double > p;
        TS_GUM_ASSERT_THROWS_NOTHING(p = ie.posterior("min"));
        gum::Instantiation I(p);
        for (I.setFirst(); !I.end(); I.inc())
          TS_ASSERT_EQUALS(p.get(I), I.val(0) == 3 ? 1 : 0);
      }
      {
        gum::VariableElimination< double > ie(&bn);
        ie.makeInference();
        gum::Potential< double > p;
        TS_GUM_ASSERT_THROWS_NOTHING(p = ie.posterior("min"));
        gum::Instantiation I(p);
        for (I.setFirst(); !I.end(); I.inc())
          TS_ASSERT_EQUALS(p.get(I), I.val(0) == 3 ? 1 : 0);
      }
      {
        gum::LoopyBeliefPropagation< double > ie(&bn);
        ie.makeInference();
        gum::Potential< double > p;
        TS_GUM_ASSERT_THROWS_NOTHING(p = ie.posterior("min"));
        gum::Instantiation I(p);
        for (I.setFirst(); !I.end(); I.inc())
          TS_ASSERT_EQUALS(p.get(I), I.val(0) == 3 ? 1 : 0);
      }
      // if (joint->sum()!=1)  // hard test for ReadOnly CPT (as aggregator)
    }
  };
}   // namespace gum_tests
