
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/loopySamplingInference.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/variables/labelizedVariable.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class CompareInferencesTestSuite: public CxxTest::TestSuite {
    private:
    // Builds a BN to test the inference
    void fill_bn1(gum::BayesNet< double >& bn) {
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

      bn.cpt(i1).fillWith({0.2, 0.8});
      bn.cpt(i2).fillWith({0.3, 0.7});
      bn.cpt(i3).fillWith({0.1, 0.9, 0.9, 0.1});
      bn.cpt(i4).fillWith(   // clang-format off
              {0.4, 0.6,
               0.5, 0.5,
               0.5, 0.5,
               1.0, 0.0} );   // clang-format on
      bn.cpt(i5)
         .fillWith(   // clang-format off
              {0.3 , 0.6 , 0.1,
               0.5 , 0.4 , 0.1,
               0.4 , 0.5 , 0.1,
               0.9 , 0.05, 0.05,
               0.4 , 0.5 , 0.1,
               0.5 , 0.45, 0.05,
               0.45, 0.5 , 0.05,
               0.1 , 0.1 , 0.8} );  // clang-format
                                                                       // on
    }

    public:
    gum::BayesNet< double >* bn;
    gum::NodeId              i1, i2, i3, i4, i5;

    void setUp() {
      bn = new gum::BayesNet< double >();

      fill_bn1(*bn);
    }

    void tearDown() { delete (bn); }

    void testInferencesWithNoEvidence() {
      try {
        begin_test_waiting();

        gum::ShaferShenoyInference< double > inf_ShaShe(bn);
        test_waiting();
        inf_ShaShe.makeInference();

        gum::LazyPropagation< double > inf_LazyProp(bn);
        test_waiting();
        inf_LazyProp.makeInference();

        gum::VariableElimination< double > inf_ValElim(bn);
        test_waiting();
        inf_ValElim.makeInference();

        gum::HybridImportanceSampling< double > inf_gibbs(bn);
        inf_gibbs.setVerbosity(false);
        inf_gibbs.setEpsilon(1e-3);
        inf_gibbs.setMinEpsilonRate(1e-3);
        test_waiting();
        TS_GUM_ASSERT_THROWS_NOTHING(inf_gibbs.makeInference());

        for (const auto i : bn->nodes()) {
          const gum::Potential< double >& marginal_gibbs = inf_gibbs.posterior(i);
          const gum::Potential< double >& marginal_ShaShe =
            inf_ShaShe.posterior(i);
          const gum::Potential< double >& marginal_LazyProp =
            inf_LazyProp.posterior(i);
          const gum::Potential< double >& marginal_ValElim =
            inf_ValElim.posterior(i);

          gum::Instantiation I;
          I << bn->variable(i);

          for (I.setFirst(); !I.end(); ++I) {
            TS_ASSERT_DELTA(marginal_gibbs[I],
                            marginal_ShaShe[I],
                            5e-2);  // APPROX INFERENCE
            TS_ASSERT_DELTA(marginal_LazyProp[I],
                            marginal_ShaShe[I],
                            1e-10);  // EXACT INFERENCE
            TS_ASSERT_DELTA(marginal_LazyProp[I],
                            marginal_ValElim[I],
                            1e-10);  // EXACT INFERENCE
            TS_ASSERT_DELTA(marginal_ShaShe[I],
                            marginal_ValElim[I],
                            1e-10);  // EXACT INFERENCE
          }
        }
        end_test_waiting();
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
      }
    }

    void testInferencesWithHardEvidence() {
      begin_test_waiting();
      gum::Potential< double > e_i1;
      e_i1 << bn->variable(i1);
      e_i1.fillWith({1, 0});

      gum::Potential< double > e_i4;
      e_i4 << bn->variable(i4);
      e_i4.fillWith({0, 1});

      gum::List< gum::Potential< double > const* > list_pot;
      list_pot.insert(&e_i1);
      list_pot.insert(&e_i4);

      gum::ShaferShenoyInference< double > inf_ShaShe(bn);
      for (auto pot : list_pot)
        inf_ShaShe.addEvidence(*pot);
      test_waiting();
      inf_ShaShe.makeInference();

      gum::LazyPropagation< double > inf_LazyProp(bn);
      for (auto pot : list_pot)
        inf_LazyProp.addEvidence(*pot);
      test_waiting();
      inf_LazyProp.makeInference();

      gum::VariableElimination< double > inf_VarElim(bn);
      for (auto pot : list_pot)
        inf_VarElim.addEvidence(*pot);
      test_waiting();
      inf_VarElim.makeInference();

      gum::HybridImportanceSampling< double > inf_his(bn);
      for (auto pot : list_pot)
        inf_his.addEvidence(*pot);
      inf_his.setVerbosity(false);
      inf_his.setEpsilon(1e-3);
      inf_his.setMinEpsilonRate(1e-3);
      test_waiting();
      TS_GUM_ASSERT_THROWS_NOTHING(inf_his.makeInference());

      for (const auto i : bn->nodes()) {
        const gum::Potential< double >& marginal_gibbs = inf_his.posterior(i);
        const gum::Potential< double >& marginal_ShaShe = inf_ShaShe.posterior(i);
        const gum::Potential< double >& marginal_LazyProp =
          inf_LazyProp.posterior(i);
        const gum::Potential< double >& marginal_VarElim =
          inf_VarElim.posterior(i);

        gum::Instantiation I;
        I << bn->variable(i);

        for (I.setFirst(); !I.end(); ++I) {
          TS_ASSERT_DELTA(marginal_gibbs[I],
                          marginal_ShaShe[I],
                          1e-1);  // APPROX INFERENCE
          TS_ASSERT_DELTA(marginal_LazyProp[I],
                          marginal_ShaShe[I],
                          1e-10);  // EXACT INFERENCE
          TS_ASSERT_DELTA(marginal_LazyProp[I],
                          marginal_VarElim[I],
                          1e-10);  // EXACT INFERENCE
          TS_ASSERT_DELTA(marginal_ShaShe[I],
                          marginal_VarElim[I],
                          1e-10);  // EXACT INFERENCE
        }
      }
      end_test_waiting();
    }

    void testInferencesWithSoftEvidence() {
      begin_test_waiting();

      gum::Potential< double > e_i1;
      e_i1 << bn->variable(i1);
      e_i1.fillWith({0.6, 1});

      gum::Potential< double > e_i4;
      e_i4 << bn->variable(i4);
      e_i4.fillWith({0.2, 0.3});

      gum::List< gum::Potential< double > const* > list_pot;
      list_pot.insert(&e_i1);
      list_pot.insert(&e_i4);

      gum::ShaferShenoyInference< double > inf_ShaShe(bn);
      for (auto pot : list_pot)
        inf_ShaShe.addEvidence(*pot);
      test_waiting();
      inf_ShaShe.makeInference();

      gum::LazyPropagation< double > inf_LazyProp(bn);
      for (auto pot : list_pot)
        inf_LazyProp.addEvidence(*pot);
      test_waiting();
      inf_LazyProp.makeInference();

      gum::VariableElimination< double > inf_VarElim(bn);
      for (auto pot : list_pot)
        inf_VarElim.addEvidence(*pot);
      test_waiting();
      inf_VarElim.makeInference();

      for (const auto i : bn->nodes()) {
        const gum::Potential< double >& marginal_ShaShe = inf_ShaShe.posterior(i);
        const gum::Potential< double >& marginal_LazyProp =
          inf_LazyProp.posterior(i);
        const gum::Potential< double >& marginal_VarElim =
          inf_VarElim.posterior(i);

        gum::Instantiation I;
        I << bn->variable(i);

        for (I.setFirst(); !I.end(); ++I) {
          TS_ASSERT_DELTA(marginal_LazyProp[I],
                          marginal_ShaShe[I],
                          1e-10);  // EXACT INFERENCE
          TS_ASSERT_DELTA(marginal_LazyProp[I],
                          marginal_VarElim[I],
                          1e-10);  // EXACT INFERENCE
          TS_ASSERT_DELTA(marginal_ShaShe[I],
                          marginal_VarElim[I],
                          1e-10);  // EXACT INFERENCE
        }
      }
      end_test_waiting();
    }

  void testMultipleInference() {
      try {
        gum::BayesNet< double > bn;
        gum::NodeId            c, s, r, w;

        gum::LabelizedVariable vc("c", "cloudy", 2), vs("s", "sprinklet", 2);
        gum::LabelizedVariable vr("r", "rain", 2), vw("w", "wet grass", 2);

        c = bn.add(vc);
        s = bn.add(vs);
        r = bn.add(vr);
        w = bn.add(vw);

        bn.addArc(c, s);
        bn.addArc(c, r);
        bn.addArc(s, w);
        bn.addArc(r, w);

        bn.cpt(c).fillWith({0.5f, 0.5f});
        bn.cpt(s).fillWith({0.5f, 0.5f, 0.9f, 0.1f});
        bn.cpt(r).fillWith({0.8f, 0.2f, 0.2f, 0.8f});
        bn.cpt(w).fillWith({1.0f, 0.0f, 0.1f, 0.9f, 0.1f, 0.9f, 0.01f, 0.99f});

        gum::Potential< double > e_i1;
        e_i1 << bn.variable(c);
        e_i1.fillWith({1, 0});

        gum::Potential< double > e_i4;
        e_i4 << bn.variable(s);
        e_i4.fillWith({0, 1});

        gum::List< gum::Potential< double > const* > list_pot;
        list_pot.insert(&e_i1);
        list_pot.insert(&e_i4);

        {
          gum::LazyPropagation< double > inf(&bn);
          inf.makeInference();
          {
            const gum::Potential< double >& p = inf.posterior(w);
            gum::Instantiation             I(p);
            TS_ASSERT_DELTA(p[I], 0.3529f, 1e-7f);
            ++I;
            TS_ASSERT_DELTA(p[I], 0.6471f, 1e-7f);
          }

          inf.eraseAllEvidence();
          for (auto pot : list_pot)
            inf.addEvidence(*pot);
          inf.makeInference();
          {
            const gum::Potential< double >& p = inf.posterior(w);
            gum::Instantiation             I(p);
            TS_ASSERT_DELTA(p[I], 0.082f, 1e-7f);
            ++I;
            TS_ASSERT_DELTA(p[I], 0.918f, 1e-7f);
          }
        }

        {
          gum::ShaferShenoyInference< double > inf(&bn);
          inf.makeInference();
          {
            const gum::Potential< double >& p = inf.posterior(w);
            gum::Instantiation             I(p);
            TS_ASSERT_DELTA(p[I], 0.3529f, 1e-5f);
            ++I;
            TS_ASSERT_DELTA(p[I], 0.6471f, 1e-5f);
          }

          inf.eraseAllEvidence();
          for (auto pot : list_pot)
            inf.addEvidence(*pot);
          inf.makeInference();
          {
            const gum::Potential< double >& p = inf.posterior(w);
            gum::Instantiation             I(p);
            TS_ASSERT_DELTA(p[I], 0.082f, 1e-7f);
            ++I;
            TS_ASSERT_DELTA(p[I], 0.918f, 1e-7f);
          }
        }

        {
          gum::VariableElimination< double > inf(&bn);
          inf.makeInference();
          {
            const gum::Potential< double >& p = inf.posterior(w);
            gum::Instantiation             I(p);
            TS_ASSERT_DELTA(p[I], 0.3529f, 1e-5f);
            ++I;
            TS_ASSERT_DELTA(p[I], 0.6471f, 1e-5f);
          }

          inf.eraseAllEvidence();
          for (auto pot : list_pot)
            inf.addEvidence(*pot);
          inf.makeInference();
          {
            const gum::Potential< double >& p = inf.posterior(w);
            gum::Instantiation             I(p);
            TS_ASSERT_DELTA(p[I], 0.082f, 1e-7f);
            ++I;
            TS_ASSERT_DELTA(p[I], 0.918f, 1e-7f);
          }
        }
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
      }
    }


  // compare only Lazy and ShaferShenoy on alarm BN
  void
  testAlarm() {
    // Arrange
    std::string              file = GET_RESSOURCES_PATH("alarm.bif");
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, file);
    TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed());
    gum::VariableElimination< double >   ve(&bn);
    gum::ShaferShenoyInference< double > ss(&bn);
    gum::LazyPropagation< double >       lazy(&bn);
    gum::Potential< double >             p_ve, p_ss, p_lazy;
    for (auto var_id : bn.nodes()) {
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(p_ve = ve.posterior(var_id));
      TS_GUM_ASSERT_THROWS_NOTHING(p_ss = ss.posterior(var_id));
      TS_GUM_ASSERT_THROWS_NOTHING(p_lazy = lazy.posterior(var_id));
      // Assert
      TS_ASSERT_EQUALS(p_ve.domainSize(), p_ss.domainSize());
      TS_ASSERT_EQUALS(p_ve.domainSize(), p_lazy.domainSize());
      for (gum::Instantiation i(p_ve); !i.end(); i.inc()) {
        TS_ASSERT_DELTA(p_ve[i], p_ss[i], 1e-6);
        TS_ASSERT_DELTA(p_ve[i], p_lazy[i], 1e-6);
      }
    }
  }

  void testAlarmWithHardEvidence() {
    // Arrange
    std::string              file = GET_RESSOURCES_PATH("alarm.bif");
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, file);
    TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed());
    gum::VariableElimination< double >   ve(&bn);
    gum::ShaferShenoyInference< double > ss(&bn);
    gum::LazyPropagation< double >       lazy(&bn);
    gum::Potential< double >             p_ve, p_ss, p_lazy;
    auto                                 e_id = bn.idFromName("CATECHOL");
    std::vector< gum::BayesNetInference< double >* > inf_list{&ve, &lazy, &ss};
    for (auto inf : inf_list) {
      inf->addEvidence(e_id, 0);
    }
    for (auto var_id : bn.nodes()) {
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(p_ve = ve.posterior(var_id));
      TS_GUM_ASSERT_THROWS_NOTHING(p_ss = ss.posterior(var_id));
      TS_GUM_ASSERT_THROWS_NOTHING(p_lazy = lazy.posterior(var_id));
      // Assert
      TS_ASSERT_EQUALS(p_ve.domainSize(), p_ss.domainSize());
      TS_ASSERT_EQUALS(p_ve.domainSize(), p_lazy.domainSize());
      for (gum::Instantiation i(p_ve); !i.end(); i.inc()) {
        TS_ASSERT_DELTA(p_ve[i], p_ss[i], 1e-6);
        TS_ASSERT_DELTA(p_ve[i], p_lazy[i], 1e-6);
      }
    }
  }

  void testAlarmWithSoftEvidence() {
    // Arrange
    std::string              file = GET_RESSOURCES_PATH("alarm.bif");
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, file);
    TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed());
    gum::VariableElimination< double >   ve(&bn);
    gum::ShaferShenoyInference< double > ss(&bn);
    gum::LazyPropagation< double >       lazy(&bn);
    gum::Potential< double >             p_ve, p_ss, p_lazy;
    auto                                 e_id = bn.idFromName("CATECHOL");
    auto                                 e_p = gum::Potential< double >();
    e_p.add(bn.variable(e_id));
    e_p.fillWith(std::vector< double >{0.1, 0.9});
    gum::List< const gum::Potential< double >* > list;
    list.insert(&e_p);
    auto inf_list =
      std::vector< gum::BayesNetInference< double >* >{&ve, &lazy, &ss};
    for (auto inf : inf_list) {
      for (auto ev : list) {
        inf->addEvidence(*ev);
      }
    }

    for (auto var_id : bn.nodes()) {
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(p_ve = ve.posterior(var_id));
      TS_GUM_ASSERT_THROWS_NOTHING(p_ss = ss.posterior(var_id));
      TS_GUM_ASSERT_THROWS_NOTHING(p_lazy = lazy.posterior(var_id));
      // Assert
      TS_ASSERT_EQUALS(p_ve.domainSize(), p_ss.domainSize());
      TS_ASSERT_EQUALS(p_ve.domainSize(), p_lazy.domainSize());

      gum::Instantiation i_ve(p_ve);
      for (gum::Instantiation i(p_ve); !i.end(); i.inc()) {
        TS_ASSERT_DELTA(p_ve[i], p_ss[i], 1e-6);
        TS_ASSERT_DELTA(p_ve[i], p_lazy[i], 1e-6);
      }
    }
  }

  void testAsia() {
    // Arrange
    std::string              file = GET_RESSOURCES_PATH("asia.bif");
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, file);
    TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed());
    gum::VariableElimination< double >   ve(&bn);
    gum::ShaferShenoyInference< double > ss(&bn);
    gum::LazyPropagation< double >       lazy(&bn);
    gum::Potential< double >             p_ve, p_ss, p_lazy;
    for (auto var_id : bn.nodes()) {
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(p_ve = ve.posterior(var_id));
      TS_GUM_ASSERT_THROWS_NOTHING(p_ss = ss.posterior(var_id));
      TS_GUM_ASSERT_THROWS_NOTHING(p_lazy = lazy.posterior(var_id));
      // Assert
      TS_ASSERT_EQUALS(p_ve.domainSize(), p_ss.domainSize());
      TS_ASSERT_EQUALS(p_ve.domainSize(), p_lazy.domainSize());

      gum::Instantiation i_ve(p_ve);
      for (gum::Instantiation i(p_ve); !i.end(); i.inc()) {
        TS_ASSERT_DELTA(p_ve[i], p_ss[i], 1e-6);
        TS_ASSERT_DELTA(p_ve[i], p_lazy[i], 1e-6);
      }
    }
  }

  void testAsiaWithHardEvidence() {
    // Arrange
    std::string              file = GET_RESSOURCES_PATH("asia.bif");
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, file);
    TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed());
    gum::VariableElimination< double >   ve(&bn);
    gum::ShaferShenoyInference< double > ss(&bn);
    gum::LazyPropagation< double >       lazy(&bn);
    gum::Potential< double >             p_ve, p_ss, p_lazy;
    auto                                 e_id = bn.idFromName("bronchitis");
    auto                                 inf_list =
      std::vector< gum::BayesNetInference< double >* >{&ve, &lazy, &ss};
    for (auto inf : inf_list) {
      inf->addEvidence(e_id, 0);
    }
    for (auto var_id : bn.nodes()) {
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(p_ve = ve.posterior(var_id));
      TS_GUM_ASSERT_THROWS_NOTHING(p_ss = ss.posterior(var_id));
      TS_GUM_ASSERT_THROWS_NOTHING(p_lazy = lazy.posterior(var_id));
      // Assert
      TS_ASSERT_EQUALS(p_ve.domainSize(), p_ss.domainSize());
      TS_ASSERT_EQUALS(p_ve.domainSize(), p_lazy.domainSize());

      gum::Instantiation i_ve(p_ve);
      for (gum::Instantiation i(p_ve); !i.end(); i.inc()) {
        TS_ASSERT_DELTA(p_ve[i], p_ss[i], 1e-6);
        TS_ASSERT_DELTA(p_ve[i], p_lazy[i], 1e-6);
      }
    }
  }

  void testAsiaWithSoftEvidence() {
    // Arrange
    std::string              file = GET_RESSOURCES_PATH("asia.bif");
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, file);
    TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed());
    gum::VariableElimination< double >   ve(&bn);
    gum::ShaferShenoyInference< double > ss(&bn);
    gum::LazyPropagation< double >       lazy(&bn);
    gum::Potential< double >             p_ve, p_ss, p_lazy;
    auto                                 e_id = bn.idFromName("bronchitis");
    auto                                 e_p = gum::Potential< double >();
    e_p.add(bn.variable(e_id));
    e_p.fillWith(std::vector< double >{0.1, 0.9});
    gum::List< const gum::Potential< double >* > list;
    list.insert(&e_p);
    auto inf_list =
      std::vector< gum::BayesNetInference< double >* >{&ve, &lazy, &ss};
    for (auto inf : inf_list) {
      for (auto ev : list) {
        inf->addEvidence(*ev);
      }
    }

    for (auto var_id : bn.nodes()) {
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(p_ve = ve.posterior(var_id));
      TS_GUM_ASSERT_THROWS_NOTHING(p_ss = ss.posterior(var_id));
      TS_GUM_ASSERT_THROWS_NOTHING(p_lazy = lazy.posterior(var_id));
      // Assert
      TS_ASSERT_EQUALS(p_ve.domainSize(), p_ss.domainSize());
      TS_ASSERT_EQUALS(p_ve.domainSize(), p_lazy.domainSize());

      gum::Instantiation i_ve(p_ve);
      for (gum::Instantiation i(p_ve); !i.end(); i.inc()) {
        TS_ASSERT_DELTA(p_ve[i], p_ss[i], 1e-6);
        TS_ASSERT_DELTA(p_ve[i], p_lazy[i], 1e-6);
      }
    }
  }
};
}
