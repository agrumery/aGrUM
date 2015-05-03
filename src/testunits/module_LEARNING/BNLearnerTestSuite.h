/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
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
// floating point env
#include <cfenv>

#include <iostream>
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/learning/BNLearner.h>

#include <agrum/core/algorithms/approximationScheme/approximationSchemeListener.h>

namespace gum_tests {

  class aSimpleBNLeanerListener : public gum::ApproximationSchemeListener {
    private:
    int __nbr;
    std::string __mess;

    public:
    aSimpleBNLeanerListener(gum::IApproximationSchemeConfiguration &sch)
        : gum::ApproximationSchemeListener(sch), __nbr(0), __mess(""){};
    void whenProgress(const void *buffer, gum::Size a, double b, double c) {
      __nbr++;
    }
    void whenStop(const void *buffer, std::string s) { __mess = s; }

    int getNbr() { return __nbr; }
    std::string getMess() { return __mess; }
  };

  class BNLearnerTestSuite : public CxxTest::TestSuite {
    public:
    void test_asia() {
      gum::learning::BNLearner<float> learner(GET_PATH_STR("asia3.csv"));

      learner.useLocalSearchWithTabuList(100, 1);
      learner.setMaxIndegree(10);
      learner.useScoreLog2Likelihood();

      TS_ASSERT_THROWS(learner.useScoreBD(), gum::IncompatibleScoreApriori);
      TS_GUM_ASSERT_THROWS_NOTHING(learner.useScoreBDeu());
      learner.useScoreLog2Likelihood();

      learner.useK2(std::vector<gum::NodeId>{1, 5, 2, 6, 0, 3, 4, 7});
      // learner.addForbiddenArc ( gum::Arc (4,3) );
      // learner.addForbiddenArc ( gum::Arc (5,1) );
      // learner.addForbiddenArc ( gum::Arc (5,7) );

      // learner.addMandatoryArc ( gum::Arc ( learner.nodeId ( "bronchitis?" ),
      //                                      learner.nodeId ( "lung_cancer?" ) ) );

      learner.addMandatoryArc("bronchitis?", "lung_cancer?");

      learner.useAprioriSmoothing();
      learner.setAprioriWeight(1);
      // learner.useAprioriDirichlet (  GET_PATH_STR( "asia.csv" ) );

      gum::NodeProperty<unsigned int> slice_order{std::make_pair(gum::NodeId(0), 1),
                                                  std::make_pair(gum::NodeId(3), 0),
                                                  std::make_pair(gum::NodeId(1), 0)};
      // learner.setSliceOrder ( slice_order );

      const std::vector<std::string> &names = learner.names();
      TS_ASSERT(!names.empty());

      try {
        gum::BayesNet<float> bn = learner.learnBN();
        TS_ASSERT(bn.dag().arcs().size() == 9);
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }

    void test_asia_with_user_modalities_string() {
      gum::NodeProperty<gum::Sequence<std::string>> modals;
      modals.insert(0, gum::Sequence<std::string>());
      modals[0].insert("false");
      modals[0].insert("true");
      modals[0].insert("big");

      modals.insert(2, gum::Sequence<std::string>());
      modals[2].insert("big");
      modals[2].insert("bigbig");
      modals[2].insert("true");
      modals[2].insert("bigbigbig");
      modals[2].insert("false");

      gum::learning::BNLearner<float> learner(GET_PATH_STR("asia3.csv"), modals);

      learner.useGreedyHillClimbing();
      learner.setMaxIndegree(10);
      learner.useScoreLog2Likelihood();

      TS_ASSERT_THROWS(learner.useScoreBD(), gum::IncompatibleScoreApriori);
      TS_GUM_ASSERT_THROWS_NOTHING(learner.useScoreBDeu());
      learner.useScoreLog2Likelihood();

      learner.useK2(std::vector<gum::NodeId>{1, 5, 2, 6, 0, 3, 4, 7});
      // learner.addForbiddenArc ( gum::Arc (4,3) );
      // learner.addForbiddenArc ( gum::Arc (5,1) );
      // learner.addForbiddenArc ( gum::Arc (5,7) );

      // learner.addMandatoryArc ( gum::Arc ( learner.nodeId ( "bronchitis?" ),
      //                                      learner.nodeId ( "lung_cancer?" ) ) );

      learner.addMandatoryArc("bronchitis?", "lung_cancer?");

      learner.useAprioriSmoothing();
      learner.setAprioriWeight(1);
      // learner.useAprioriDirichlet (  GET_PATH_STR( "asia.csv" ) );

      gum::NodeProperty<unsigned int> slice_order{std::make_pair(gum::NodeId(0), 1),
                                                  std::make_pair(gum::NodeId(3), 0),
                                                  std::make_pair(gum::NodeId(1), 0)};
      // learner.setSliceOrder ( slice_order );

      const std::vector<std::string> &names = learner.names();
      TS_ASSERT(!names.empty());

      try {
        gum::BayesNet<float> bn = learner.learnBN();
        TS_ASSERT(bn.variable(0).domainSize() == 3);
        TS_ASSERT(bn.variable(2).domainSize() == 5);
        TS_ASSERT(bn.variable(0).label(0) == "false");
        TS_ASSERT(bn.variable(0).label(1) == "true");
        TS_ASSERT(bn.variable(0).label(2) == "big");
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }

    void test_asia_with_user_modalities_string_min() {
      gum::NodeProperty<gum::Sequence<std::string>> modals;
      modals.insert(0, gum::Sequence<std::string>());
      modals[0].insert("false");
      modals[0].insert("true");
      modals[0].insert("big");

      modals.insert(2, gum::Sequence<std::string>());
      modals[2].insert("big");
      modals[2].insert("bigbig");
      modals[2].insert("true");
      modals[2].insert("bigbigbig");
      modals[2].insert("false");

      gum::learning::BNLearner<float> learner(GET_PATH_STR("asia3.csv"), modals,
                                              true);

      learner.useGreedyHillClimbing();
      learner.setMaxIndegree(10);
      learner.useScoreLog2Likelihood();

      TS_ASSERT_THROWS(learner.useScoreBD(), gum::IncompatibleScoreApriori);
      TS_GUM_ASSERT_THROWS_NOTHING(learner.useScoreBDeu());
      learner.useScoreLog2Likelihood();

      learner.useK2(std::vector<gum::NodeId>{1, 5, 2, 6, 0, 3, 4, 7});
      // learner.addForbiddenArc ( gum::Arc (4,3) );
      // learner.addForbiddenArc ( gum::Arc (5,1) );
      // learner.addForbiddenArc ( gum::Arc (5,7) );

      // learner.addMandatoryArc ( gum::Arc ( learner.nodeId ( "bronchitis?" ),
      //                                      learner.nodeId ( "lung_cancer?" ) ) );

      learner.addMandatoryArc("bronchitis?", "lung_cancer?");

      learner.useAprioriSmoothing();
      learner.setAprioriWeight(1);
      // learner.useAprioriDirichlet (  GET_PATH_STR( "asia.csv" ) );

      gum::NodeProperty<unsigned int> slice_order{std::make_pair(gum::NodeId(0), 1),
                                                  std::make_pair(gum::NodeId(3), 0),
                                                  std::make_pair(gum::NodeId(1), 0)};
      // learner.setSliceOrder ( slice_order );

      const std::vector<std::string> &names = learner.names();
      TS_ASSERT(!names.empty());

      try {
        gum::BayesNet<float> bn = learner.learnBN();
        TS_ASSERT(bn.variable(0).domainSize() == 2);
        TS_ASSERT(bn.variable(2).domainSize() == 2);
        TS_ASSERT(bn.variable(0).label(0) == "false");
        TS_ASSERT(bn.variable(0).label(1) == "true");
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }

    void test_asia_with_user_modalities_string_incorrect() {
      gum::NodeProperty<gum::Sequence<std::string>> modals;
      modals.insert(0, gum::Sequence<std::string>());
      modals[0].insert("False");
      modals[0].insert("true");
      modals[0].insert("big");

      modals.insert(2, gum::Sequence<std::string>());
      modals[2].insert("big");
      modals[2].insert("bigbig");
      modals[2].insert("true");
      modals[2].insert("bigbigbig");
      modals[2].insert("false");

      bool except = false;

      try {
        gum::learning::BNLearner<float> learner(GET_PATH_STR("asia3.csv"), modals);
        learner.useAprioriSmoothing();
      } catch (gum::UnknownLabelInDatabase &e) {
        except = true;
      }

      TS_ASSERT(except);
    }

    void test_asia_with_user_modalities_numbers() {
      gum::NodeProperty<gum::Sequence<std::string>> modals;
      modals.insert(0, gum::Sequence<std::string>());
      modals[0].insert("0");
      modals[0].insert("1");
      modals[0].insert("big");

      modals.insert(2, gum::Sequence<std::string>());
      modals[2].insert("big");
      modals[2].insert("bigbig");
      modals[2].insert("1");
      modals[2].insert("bigbigbig");
      modals[2].insert("0");

      gum::learning::BNLearner<float> learner(GET_PATH_STR("asia.csv"), modals);
      learner.useGreedyHillClimbing();
      learner.setMaxIndegree(10);
      learner.useScoreLog2Likelihood();

      TS_ASSERT_THROWS(learner.useScoreBD(), gum::IncompatibleScoreApriori);
      TS_GUM_ASSERT_THROWS_NOTHING(learner.useScoreBDeu());
      learner.useScoreLog2Likelihood();

      learner.useK2(std::vector<gum::NodeId>{1, 5, 2, 6, 0, 3, 4, 7});
      // learner.addForbiddenArc ( gum::Arc (4,3) );
      // learner.addForbiddenArc ( gum::Arc (5,1) );
      // learner.addForbiddenArc ( gum::Arc (5,7) );

      // learner.addMandatoryArc ( gum::Arc ( learner.nodeId ( "bronchitis?" ),
      //                                      learner.nodeId ( "lung_cancer?" ) ) );

      learner.addMandatoryArc("bronchitis?", "lung_cancer?");

      learner.useAprioriSmoothing();
      learner.setAprioriWeight(1);
      // learner.useAprioriDirichlet (  GET_PATH_STR( "asia.csv" ) );

      gum::NodeProperty<unsigned int> slice_order{std::make_pair(gum::NodeId(0), 1),
                                                  std::make_pair(gum::NodeId(3), 0),
                                                  std::make_pair(gum::NodeId(1), 0)};
      // learner.setSliceOrder ( slice_order );

      const std::vector<std::string> &names = learner.names();
      TS_ASSERT(!names.empty());

      try {
        gum::BayesNet<float> bn = learner.learnBN();
        TS_ASSERT(bn.variable(0).domainSize() == 3);
        TS_ASSERT(bn.variable(2).domainSize() == 5);
        TS_ASSERT(bn.variable(0).label(0) == "0");
        TS_ASSERT(bn.variable(0).label(1) == "1");
        TS_ASSERT(bn.variable(0).label(2) == "big");
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }

    void test_asia_with_user_modalities_numbers_incorrect() {
      gum::NodeProperty<gum::Sequence<std::string>> modals;
      modals.insert(0, gum::Sequence<std::string>());
      modals[0].insert("1");
      modals[0].insert("2");
      modals[0].insert("big");

      modals.insert(2, gum::Sequence<std::string>());
      modals[2].insert("big");
      modals[2].insert("bigbig");
      modals[2].insert("3");
      modals[2].insert("bigbigbig");
      modals[2].insert("0");

      bool except = false;

      try {
        gum::learning::BNLearner<float> learner(GET_PATH_STR("asia.csv"), modals);
        learner.useAprioriSmoothing();
      } catch (gum::UnknownLabelInDatabase &e) {
        except = true;
      }

      TS_ASSERT(except);
    }

    void test_asia_param() {
      gum::learning::BNLearner<float> learner(GET_PATH_STR("asia3.csv"));

      gum::DAG dag;

      for (unsigned int i = 0; i < 8; ++i) {
        dag.addNode(i);
      }

      for (unsigned int i = 0; i < 7; ++i) {
        dag.addArc(i, i + 1);
      }

      dag.addArc(0, 7);
      dag.addArc(2, 4);
      dag.addArc(5, 7);
      dag.addArc(3, 6);

      learner.useNoApriori();

      try {
        gum::BayesNet<float> bn = learner.learnParameters(dag);
        TS_ASSERT(bn.dim() == 25);
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }

    void test_asia_param_from_bn() {
      gum::learning::BNLearner<float> learner(GET_PATH_STR("asia3.csv"));

      learner.useK2(std::vector<gum::NodeId>{1, 5, 2, 6, 0, 3, 4, 7});
      gum::BayesNet<float> bn = learner.learnBN();

      try {
        gum::BayesNet<float> bn2 = learner.learnParameters(bn);
        TS_ASSERT(bn2.dag().arcs().size() == bn.dag().arcs().size());
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }

    void test_asia_param_bn() {
#define createBoolVar(s)                                                            \
  gum::LabelizedVariable(s, s, 0).addLabel("false").addLabel("true");
      // smoking?,lung_cancer?,bronchitis?,visit_to_Asia?,tuberculosis?,tuberculos_or_cancer?,dyspnoea?,positive_XraY?
      auto s = createBoolVar("smoking?");
      auto l = createBoolVar("lung_cancer?");
      auto b = createBoolVar("bronchitis?");
      auto v = createBoolVar("visit_to_Asia?");
      auto t = createBoolVar("tuberculosis?");
      auto o = createBoolVar("tuberculos_or_cancer?");
      auto d = createBoolVar("dyspnoea?");
      auto p = createBoolVar("positive_XraY?");
#undef createBoolVar

      gum::BayesNet<float> bn;
      gum::NodeId ns = bn.add(s);
      gum::NodeId nl = bn.add(l);
      gum::NodeId nb = bn.add(b);
      gum::NodeId nv = bn.add(v);
      gum::NodeId nt = bn.add(t);
      gum::NodeId no = bn.add(o);
      gum::NodeId nd = bn.add(d);
      gum::NodeId np = bn.add(p);

      bn.addArc(ns, nl);
      bn.addArc(ns, nb);
      bn.addArc(nl, no);
      bn.addArc(nb, nd);
      bn.addArc(nv, nt);
      bn.addArc(nt, no);
      bn.addArc(no, nd);
      bn.addArc(no, np);

      gum::learning::BNLearner<float> learner(GET_PATH_STR("asia3.csv"), bn);

      learner.useScoreLog2Likelihood();
      learner.useAprioriSmoothing();

      try {
        gum::BayesNet<float> bn2 = learner.learnParameters(bn);
        TS_ASSERT(bn2.dim() == bn.dim());

        for (gum::NodeId node : bn.nodes()) {
          gum::NodeId node2 = bn2.idFromName(bn.variable(node).name());
          TS_ASSERT_EQUALS(bn.variable(node).toString(),
                           bn2.variable(node2).toString());
        }

      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }

    void test_asia_param_bn_with_not_matching_variable() {
#define createBoolVar(s)                                                            \
  gum::LabelizedVariable(s, s, 0).addLabel("false").addLabel("true");
      auto s = createBoolVar("smoking?");
      auto l = createBoolVar("lung_cancer?");
      auto b = createBoolVar("bronchitis?");
      auto v = createBoolVar("visit_to_Asia?");
      auto t = createBoolVar("tuberculosis?");
      auto o = createBoolVar("tuberculos_or_cancer?");
      auto d = createBoolVar("dyspnoea?");

      // uncorrect name is : will it be correctly handled ?
      auto p = createBoolVar("ZORBLOBO");
#undef createBoolVar

      gum::BayesNet<float> bn;
      bn.add(s);
      bn.add(l);
      bn.add(b);
      bn.add(v);
      bn.add(t);
      bn.add(o);
      bn.add(d);
      bn.add(p);

      gum::learning::BNLearner<float> learner(GET_PATH_STR("asia3.csv"), bn);

      learner.useScoreLog2Likelihood();
      learner.useAprioriSmoothing();

      TS_ASSERT_THROWS(gum::BayesNet<float> bn2 = learner.learnParameters(bn),
                       gum::MissingVariableInDatabase);
    }

    void test_asia_param_bn_with_subset_of_variables_in_base() {
#define createBoolVar(s)                                                            \
  gum::LabelizedVariable(s, s, 0).addLabel("false").addLabel("true");
      auto s = createBoolVar("smoking?");
      auto t = createBoolVar("tuberculosis?");
      auto o = createBoolVar("tuberculos_or_cancer?");
      auto d = createBoolVar("dyspnoea?");
#undef createBoolVar

      gum::BayesNet<float> bn;
      gum::NodeId ns = bn.add(s);
      gum::NodeId nt = bn.add(t);
      gum::NodeId no = bn.add(o);
      gum::NodeId nd = bn.add(d);

      bn.addArc(ns, nt);
      bn.addArc(nt, no);
      bn.addArc(no, nd);

      gum::learning::BNLearner<float> learner(GET_PATH_STR("asia3.csv"), bn);

      learner.useScoreLog2Likelihood();
      learner.useAprioriSmoothing();

      gum::BayesNet<float> bn2 = learner.learnParameters(bn);
    }

    void test_asia_param_bn_with_unknow_modality() {
#define createBoolVar(s)                                                            \
  gum::LabelizedVariable(s, s, 0).addLabel("false").addLabel("true");
      auto s = createBoolVar("smoking?");
      auto t = createBoolVar("tuberculosis?");
      auto o = createBoolVar("tuberculos_or_cancer?");
      auto d = createBoolVar("dyspnoea?");
#undef createBoolVar

      gum::BayesNet<float> bn;
      gum::NodeId ns = bn.add(s);
      gum::NodeId nt = bn.add(t);
      gum::NodeId no = bn.add(o);
      gum::NodeId nd = bn.add(d);

      bn.addArc(ns, nt);
      bn.addArc(nt, no);
      bn.addArc(no, nd);

      // asia3-faulty contains a label "beurk" for variable "smoking?"
      // std::cout << "error test";

      TS_ASSERT_THROWS(gum::learning::BNLearner<float> learner(
                           GET_PATH_STR("asia3-faulty.csv"), bn),
                       gum::UnknownLabelInDatabase);
    }

    void test_listener() {
      {
        gum::learning::BNLearner<double> learner(GET_PATH_STR("asia.csv"));
        aSimpleBNLeanerListener listen(learner);

        learner.setVerbosity(true);
        learner.setMaxIndegree(10);
        learner.useScoreK2();
        learner.useK2(std::vector<gum::NodeId>{1, 5, 2, 6, 0, 3, 4, 7});

        gum::BayesNet<double> bn = learner.learnBN();

        TS_ASSERT_EQUALS(listen.getNbr(), 2);
        TS_ASSERT_EQUALS(listen.getMess(), "stopped on request");
        TS_ASSERT_EQUALS(learner.messageApproximationScheme(), "stopped on request");
      }
      {
        gum::learning::BNLearner<double> learner(GET_PATH_STR("asia2.csv"));
        aSimpleBNLeanerListener listen(learner);

        learner.setVerbosity(true);
        learner.setMaxIndegree(10);
        learner.useScoreK2();
        learner.useK2(std::vector<gum::NodeId>{1, 5, 2, 6, 0, 3, 4, 7});

        gum::BayesNet<double> bn = learner.learnBN();

        TS_ASSERT_EQUALS(listen.getNbr(), 3);
        TS_ASSERT_EQUALS(listen.getMess(), "stopped on request");
        TS_ASSERT_EQUALS(learner.messageApproximationScheme(), "stopped on request");
      }
      {
        gum::learning::BNLearner<double> learner(GET_PATH_STR("asia.csv"));
        aSimpleBNLeanerListener listen(learner);

        learner.setVerbosity(true);
        learner.setMaxIndegree(2);
        learner.useLocalSearchWithTabuList();

        gum::BayesNet<double> bn = learner.learnBN();

        TS_ASSERT_EQUALS(listen.getNbr(), 75);
        TS_ASSERT_EQUALS(listen.getMess(), "stopped on request");
        TS_ASSERT_EQUALS(learner.messageApproximationScheme(), "stopped on request");
      }
      {
        gum::learning::BNLearner<double> learner(GET_PATH_STR("asia.csv"));
        aSimpleBNLeanerListener listen(learner);

        learner.setVerbosity(true);
        learner.setMaxIndegree(2);
        learner.useGreedyHillClimbing();

        gum::BayesNet<double> bn = learner.learnBN();

        TS_ASSERT_EQUALS(listen.getNbr(), 2);
        TS_ASSERT_EQUALS(listen.getMess(), "stopped on request");
        TS_ASSERT_EQUALS(learner.messageApproximationScheme(), "stopped on request");
      }
    }

    void test_DBNTonda() {
      gum::BayesNet<double> dbn;
      gum::NodeId bf_0 = dbn.add(gum::LabelizedVariable("bf_0", "bf_0", 4));
      /*gum::NodeId bf_t =*/dbn.add(gum::LabelizedVariable("bf_t", "bf_t", 4));
      gum::NodeId c_0 = dbn.add(gum::LabelizedVariable("c_0", "c_0", 5));
      gum::NodeId c_t = dbn.add(gum::LabelizedVariable("c_t", "c_t", 5));
      gum::NodeId h_0 = dbn.add(gum::LabelizedVariable("h_0", "h_0", 5));
      gum::NodeId h_t = dbn.add(gum::LabelizedVariable("h_t", "h_t", 5));
      gum::NodeId tf_0 = dbn.add(gum::LabelizedVariable("tf_0", "tf_0", 5));
      /*gum::NodeId tf_t =*/dbn.add(gum::LabelizedVariable("tf_t", "tf_t", 5));
      gum::NodeId wl_0 = dbn.add(gum::LabelizedVariable("wl_0", "wl_0", 4));
      gum::NodeId wl_t = dbn.add(gum::LabelizedVariable("wl_t", "wl_t", 4));

      for (auto n : {c_t, h_t, wl_t}) {
        dbn.addArc(tf_0, n);
        dbn.addArc(bf_0, n);
      }
      dbn.addArc(c_0, c_t);
      dbn.addArc(h_0, h_t);
      dbn.addArc(wl_0, wl_t);

      gum::BayesNet<double> learn1;
      {
        // inductive learning leads to scrambled modalities
        gum::learning::BNLearner<double> learner(GET_PATH_STR("DBN_Tonda.csv"));
        learner.useScoreLog2Likelihood();
        learner.useAprioriSmoothing(1.0);
        learn1 = learner.learnParameters(dbn);
      }
      gum::BayesNet<double> learn2;
      {
        try {
          gum::NodeProperty<gum::Sequence<std::string>> modals;
          auto ds = std::vector<unsigned int>({4, 4, 5, 5, 5, 5, 5, 5, 4, 4});
          auto labels = std::vector<std::string>({"0", "1", "2", "3", "4", "5"});

          for (auto i = 0U; i < ds.size(); i++) {
            modals.insert(i, gum::Sequence<std::string>());

            for (auto k = 0U; k < ds[i]; k++)
              modals[i].insert(labels[k]);
          }

          // while explicit learning does the right thing
          gum::learning::BNLearner<double> learner(GET_PATH_STR("DBN_Tonda.csv"),
                                                   modals);
          learner.useScoreLog2Likelihood();
          learner.useAprioriSmoothing(1.0);
          learn2 = learner.learnParameters(dbn);
        } catch (gum::Exception &e) {
          GUM_SHOWERROR(e);
        }
      }
      gum::BayesNet<double> learn3;
      {
        // while explicit learning does the right thing
        gum::learning::BNLearner<double> learner(GET_PATH_STR("DBN_Tonda.csv"), dbn);
        learner.useScoreLog2Likelihood();
        learner.useAprioriSmoothing(1.0);
        learn3 = learner.learnParameters(dbn);
      }

      TS_ASSERT_EQUALS(learn1.variable(learn1.idFromName("wl_0")).toString(),
                       "wl_0<1,0,2,3>");
      TS_ASSERT_EQUALS(learn2.variable(learn2.idFromName("wl_0")).toString(),
                       "wl_0<0,1,2,3>");
      TS_ASSERT_EQUALS(learn2.variable(learn3.idFromName("wl_0")).toString(),
                       "wl_0<0,1,2,3>");

      auto &p1 = learn1.cpt(learn1.idFromName("c_0"));
      auto &p2 = learn2.cpt(learn2.idFromName("c_0"));
      auto &p3 = learn3.cpt(learn3.idFromName("c_0"));
      gum::Instantiation I1(p1), I2(p2), I3(p3);

      for (I1.setFirst(), I2.setFirst(), I3.setFirst(); !I1.end();
           I1.inc(), I2.inc(), I3.inc()) {
        TS_ASSERT_EQUALS(I1.toString(), I2.toString()); // same modalities orders
        TS_ASSERT_EQUALS(I1.toString(), I3.toString()); // same modalities orders
        TS_ASSERT_EQUALS(p1[I1], p2[I2]);               // same probabilities
        TS_ASSERT_EQUALS(p1[I1], p3[I3]);               // same probabilities
      }
    }
  };

} /* namespace gum_tests */
