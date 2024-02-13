/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

// floating point env
#include <cfenv>
#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/tools/core/approximations/approximationSchemeListener.h>
#include <agrum/tools/variables/allDiscreteVariables.h>

#include <agrum/BN/database/BNDatabaseGenerator.h>
#include <agrum/BN/learning/BNLearner.h>

namespace gum_tests {

  class aSimpleBNLeanerListener: public gum::ApproximationSchemeListener {
    private:
    gum::Size   _nbr_  = 0;
    std::string _mess_ = "";

    public:
    using gum::ApproximationSchemeListener::ApproximationSchemeListener;

    void whenProgress(const void* buffer, const gum::Size a, const double b, const double c) final {
      _nbr_++;
    }

    void whenStop(const void* buffer, const std::string& s) final { _mess_ = s; }

    gum::Size getNbr() { return _nbr_; }

    std::string getMess() { return _mess_; }
  };

  class [[maybe_unused]] BNLearnerTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_asia) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));

      learner.useLocalSearchWithTabuList(100, 1);
      learner.setMaxIndegree(10);
      learner.useScoreLog2Likelihood();

      TS_GUM_ASSERT_THROWS_NOTHING(learner.useScoreBD())
      TS_ASSERT_DIFFERS("", learner.checkScorePriorCompatibility())
      TS_GUM_ASSERT_THROWS_NOTHING(learner.useScoreBDeu())
      TS_ASSERT_EQUALS("", learner.checkScorePriorCompatibility())
      learner.useScoreLog2Likelihood();

      learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});
      // learner.addForbiddenArc ( gum::Arc (4,3) );
      // learner.addForbiddenArc ( gum::Arc (5,1) );
      // learner.addForbiddenArc ( gum::Arc (5,7) );

      // learner.addMandatoryArc ( gum::Arc ( learner.nodeId ( "bronchitis" ),
      //                                      learner.nodeId ( "lung_cancer" )
      //                                      ) );

      learner.addMandatoryArc("bronchitis", "lung_cancer");

      learner.useSmoothingPrior();
      // learner.useDirichletPrior (  GET_RESSOURCES_PATH( "asia.csv" ) );

      gum::NodeProperty< gum::Size > slice_order{std::make_pair(gum::NodeId(0), (gum::Size)1),
                                                 std::make_pair(gum::NodeId(3), (gum::Size)0),
                                                 std::make_pair(gum::NodeId(1), (gum::Size)0)};
      learner.setSliceOrder(slice_order);

      const std::vector< std::string >& names = learner.names();
      TS_ASSERT(!names.empty())

      try {
        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.dag().arcs().size(), (gum::Size)9)
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      learner.setDatabaseWeight(10.0);
      const auto&  db     = learner.database();
      const double weight = 10.0 / double(db.nbRows());
      for (const auto& row: db) {
        TS_ASSERT_EQUALS(row.weight(), weight)
      }
      TS_ASSERT_DELTA(learner.databaseWeight(), 10.0, 1e-4)

      const std::size_t nbr = db.nbRows();
      for (auto i = std::size_t(0); i < nbr; ++i) {
        if (i % 2) learner.setRecordWeight(i, 2.0);
      }

      auto index = std::size_t(0);
      for (const auto& row: db) {
        if (index % 2) {
          TS_ASSERT_EQUALS(row.weight(), 2.0)
          TS_ASSERT_EQUALS(learner.recordWeight(index), 2.0)
        } else {
          TS_ASSERT_EQUALS(row.weight(), 10.0)
          TS_ASSERT_EQUALS(learner.recordWeight(index), 10.0)
        }
        ++index;
      }
    }

    GUM_ACTIVE_TEST(_induceTypes) {
      {
        gum::learning::BNLearner< double > learner1(GET_RESSOURCES_PATH("csv/asia.csv"));
        learner1.useScoreBDeu();
        learner1.useNoPrior();
        gum::BayesNet< double > bn1 = learner1.learnBN();

        gum::learning::BNLearner< double > learner2(GET_RESSOURCES_PATH("csv/asia.csv"),
                                                    {"?"},
                                                    true);
        for (const auto trans2: learner2.database().translatorSet().translators()) {
          const auto& var2 = trans2->variable();
          TS_ASSERT((var2->varType() == gum::VarType::Range)
                    || (var2->varType() == gum::VarType::Integer))
        }

        learner2.useScoreBDeu();
        learner2.useNoPrior();
        gum::BayesNet< double > bn2 = learner2.learnBN();

        TS_ASSERT_EQUALS(bn1.dag(), bn2.dag())
      }

      {
        gum::learning::BNLearner< double > learner1(GET_RESSOURCES_PATH("csv/alarm.csv"));
        learner1.useScoreBDeu();
        learner1.useNoPrior();
        gum::BayesNet< double > bn1 = learner1.learnBN();

        gum::learning::BNLearner< double > learner2(GET_RESSOURCES_PATH("csv/alarm.csv"),
                                                    {"?"},
                                                    true);

        for (const auto trans2: learner2.database().translatorSet().translators()) {
          const auto& var2 = trans2->variable();
          TS_ASSERT((var2->varType() == gum::VarType::Range)
                    || (var2->varType() == gum::VarType::Integer))
        }

        learner2.useScoreBDeu();
        learner2.useNoPrior();
        gum::BayesNet< double > bn2 = learner2.learnBN();

        TS_ASSERT_EQUALS(bn1.dag(), bn2.dag())
      }

      {
        auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D->E<-B");
        gum::learning::BNDatabaseGenerator< double > genere(bn);
        genere.setRandomVarOrder();
        genere.drawSamples(2000);
        genere.toCSV(GET_RESSOURCES_PATH("outputs/bnlearner_dirichlet.csv"));

        auto bn2 = gum::BayesNet< double >::fastPrototype("A->B->C->D->E");
        gum::learning::BNDatabaseGenerator< double > genere2(bn2);
        genere2.drawSamples(100);
        genere2.toCSV(GET_RESSOURCES_PATH("outputs/bnlearner_database.csv"));

        gum::learning::BNLearner< double > learner1(
           GET_RESSOURCES_PATH("outputs/bnlearner_database.csv"));
        learner1.useDirichletPrior(GET_RESSOURCES_PATH("outputs/bnlearner_dirichlet.csv"), 10);
        learner1.useScoreAIC();
        gum::BayesNet< double > xbn1 = learner1.learnBN();

        gum::learning::BNLearner< double > learner2(
           GET_RESSOURCES_PATH("outputs/bnlearner_database.csv"),
           {"?"},
           true);

        for (const auto trans2: learner2.database().translatorSet().translators()) {
          const auto& var2 = trans2->variable();
          TS_ASSERT((var2->varType() == gum::VarType::Range)
                    || (var2->varType() == gum::VarType::Integer))
        }

        learner2.useDirichletPrior(GET_RESSOURCES_PATH("outputs/bnlearner_dirichlet.csv"), 10);
        learner2.useScoreAIC();
        gum::BayesNet< double > xbn2 = learner2.learnBN();

        TS_ASSERT_EQUALS(xbn1.dag(), xbn2.dag())
      }
    }

    GUM_ACTIVE_TEST(_guill) {
      TS_ASSERT_THROWS(
         gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3_withguill.csv")),
         gum::SyntaxError&)
    }

    GUM_ACTIVE_TEST(_ranges) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));

      learner.useGreedyHillClimbing();
      learner.useScoreBIC();
      learner.useSmoothingPrior();

      const std::size_t k        = 5;
      const auto&       database = learner.database();
      const std::size_t dbsize   = database.nbRows();
      std::size_t       foldSize = dbsize / k;

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::SmoothingPrior       prior(database);
      prior.setWeight(1);

      gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintDAG >
         struct_constraint;

      gum::learning::GraphChangesGenerator4DiGraph op_set(struct_constraint);

      gum::learning::GreedyHillClimbing search;

      gum::learning::ScoreBIC         score(parser, prior);
      gum::learning::ParamEstimatorML estimator(parser, prior, score.internalPrior());
      for (std::size_t fold = 0; fold < k; fold++) {
        // create the ranges of rows over which we perform the learning
        const std::size_t unfold_deb = fold * foldSize;
        const std::size_t unfold_end = unfold_deb + foldSize;

        std::vector< std::pair< std::size_t, std::size_t > > ranges;
        if (fold == std::size_t(0)) {
          ranges.push_back(std::pair< std::size_t, std::size_t >(unfold_end, dbsize));
        } else {
          ranges.push_back(std::pair< std::size_t, std::size_t >(std::size_t(0), unfold_deb));

          if (fold != k - 1) {
            ranges.push_back(std::pair< std::size_t, std::size_t >(unfold_end, dbsize));
          }
        }

        learner.useDatabaseRanges(ranges);
        TS_ASSERT_EQUALS(learner.databaseRanges(), ranges)

        learner.clearDatabaseRanges();
        TS_ASSERT_DIFFERS(learner.databaseRanges(), ranges)

        learner.useCrossValidationFold(fold, k);
        TS_ASSERT_EQUALS(learner.databaseRanges(), ranges)

        gum::BayesNet< double > bn1 = learner.learnBN();


        score.setRanges(ranges);
        estimator.setRanges(ranges);
        gum::learning::GraphChangesSelector4DiGraph selector(score, struct_constraint, op_set);
        gum::BayesNet< double > bn2 = search.learnBN< double >(selector, estimator);

        TS_ASSERT_EQUALS(bn1.dag(), bn2.dag())

        gum::Instantiation I1;
        gum::Instantiation I2;

        for (auto& name: database.variableNames()) {
          I1.add(bn1.variableFromName(name));
          I2.add(bn2.variableFromName(name));
        }

        double            LL1   = 0.0;
        double            LL2   = 0.0;
        const std::size_t nbCol = database.nbVariables();
        parser.setRange(unfold_deb, unfold_end);
        while (parser.hasRows()) {
          const gum::learning::DBRow< gum::learning::DBTranslatedValue >& row = parser.row();
          for (std::size_t i = 0; i < nbCol; ++i) {
            I1.chgVal(i, row[i].discr_val);
            I2.chgVal(i, row[i].discr_val);
          }

          LL1 += bn1.log2JointProbability(I1) * row.weight();
          LL2 += bn2.log2JointProbability(I2) * row.weight();
        }

        TS_ASSERT_EQUALS(LL1, LL2)
      }
    }

    GUM_ACTIVE_TEST(_constraintMiic_PossibleEdges) {
      //[smoking , lung_cancer , bronchitis , visit_to_Asia , tuberculosis ,
      // tuberculos_or_cancer , dyspnoea , positive_XraY]
      // possible edges are not relevant
      try {
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
        learner.useMIIC();
        learner.addPossibleEdge("visit_to_Asia", "lung_cancer");
        learner.addPossibleEdge("visit_to_Asia", "smoking");

        auto mg = learner.learnPDAG();
        TS_ASSERT_EQUALS(mg.sizeArcs(), (gum::Size)0)
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(_asia_constraintMiic_) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia.csv"));

      aSimpleBNLeanerListener listen(learner);

      learner.useGreedyHillClimbing();
      learner.useMIIC();
      learner.useMDLCorrection();

      // Constraints
      // learner.addForbiddenArc(gum::Arc(1, 5));
      learner.addForbiddenArc(gum::Arc(2, 6));
      learner.addForbiddenArc(gum::Arc(6, 2));

      learner.addMandatoryArc(gum::Arc(3, 4));

      try {
        auto mg = learner.learnPDAG();
        // TS_ASSERT(!mg.existsArc(1, 5))
        TS_ASSERT(!mg.existsEdge(2, 6))
        TS_ASSERT(!mg.existsArc(2, 6))
        TS_ASSERT(!mg.existsArc(6, 2))

        TS_ASSERT(mg.existsArc(3, 4))
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(_census_Miic) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/Census01.csv"));

      aSimpleBNLeanerListener listen(learner);

      learner.useGreedyHillClimbing();
      learner.useMIIC();
      learner.useMDLCorrection();


      try {
        auto mg = learner.learnDAG();

      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(_alarm_constraintMiic) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/alarm.csv"));

      aSimpleBNLeanerListener listen(learner);

      learner.useGreedyHillClimbing();
      learner.useMIIC();
      learner.useNMLCorrection();

      gum::NodeProperty< gum::Size > slice_order{std::make_pair(gum::NodeId(34), (gum::Size)0),
                                                 std::make_pair(gum::NodeId(33), (gum::Size)0),
                                                 std::make_pair(gum::NodeId(29), (gum::Size)1),
                                                 std::make_pair(gum::NodeId(30), (gum::Size)1)};

      learner.setSliceOrder(slice_order);
      learner.setMaxIndegree(3);

      try {
        auto mg = learner.learnPDAG();

        gum::NodeSet nodesSet = mg.asNodeSet();
        for (auto& x: nodesSet) {
          TS_ASSERT(mg.parents(x).size() <= 3)
        }

        TS_ASSERT(!mg.existsArc(29, 33))
        TS_ASSERT(!mg.existsArc(29, 34))
        TS_ASSERT(!mg.existsArc(30, 33))
        TS_ASSERT(!mg.existsArc(30, 34))

      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    // WARNING: this test is commented on purpose: you need a running database
    // with a table filled with the content of the asia.csv file. You will also
    // need a proper odbc configuration (under linux and macos you'll need
    // unixodbc and specific database odbc drivers).
    GUM_ACTIVE_TEST(_asia_db) {
#ifdef _ODBC
      try {
        auto db = gum::learning::DatabaseFromSQL(
           "PostgreSQL",
           "lto",
           "Password2Change",
           "select smoking , lung_cancer , bronchitis , visit_to_asia , "
           "tuberculosis , tuberculos_or_cancer , dyspnoea , positive_xray "
           "from asia;");
        gum::learning::BNLearner< double > learner(db);

        learner.useLocalSearchWithTabuList(100, 1);
        learner.setMaxIndegree(10);
        learner.useScoreLog2Likelihood();

        TS_ASSERT_THROWS(learner.useScoreBD(), gum::IncompatibleScorePrior);
        TS_GUM_ASSERT_THROWS_NOTHING(learner.useScoreBDeu())
        learner.useScoreLog2Likelihood();

        learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});
        learner.addMandatoryArc("bronchitis", "lung_cancer");
        learner.useSmoothingPrior();

        gum::NodeProperty< unsigned int > slice_order{std::make_pair(gum::NodeId(0), 1),
                                                      std::make_pair(gum::NodeId(3), 0),
                                                      std::make_pair(gum::NodeId(1), 0)};

        const std::vector< std::string >& names = learner.names();
        TS_ASSERT(!names.empty())

        try {
          gum::BayesNet< double > bn = learner.learnBN();
          TS_ASSERT_EQUALS(bn.dag().arcs().size(), 9)
        } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
      } catch (gum::Exception& e) {
        GUM_TRACE(e.errorType());
        GUM_TRACE(e.errorContent());
        GUM_TRACE(e.errorCallStack());
        TS_FAIL("plop");
      }
#endif   // _ODBC
    }

    GUM_ACTIVE_TEST(_asia_with_domain_sizes) {
      gum::learning::BNLearner< double > learn(GET_RESSOURCES_PATH("csv/asia3.csv"));
      const auto&                        database = learn.database();

      gum::BayesNet< double > bn;
      for (auto& name: database.variableNames()) {
        gum::LabelizedVariable var(name, name, {"false", "true", "big"});
        bn.add(var);
      }

      gum::learning::BNLearner learner(GET_RESSOURCES_PATH("csv/asia3.csv"), bn);
      learner.useScoreBIC();
      learner.useSmoothingPrior();

      gum::BayesNet< double > bn2 = learner.learnBN();
      for (auto& name: database.variableNames()) {
        TS_ASSERT_EQUALS(bn2.variableFromName(name).domainSize(), (gum::Size)3)
      }
    }

    GUM_ACTIVE_TEST(asia_with_user_modalities_string_min) {
      gum::NodeProperty< gum::Sequence< std::string > > modals;
      modals.insert(0, gum::Sequence< std::string >());
      modals[0].insert("false");
      modals[0].insert("true");
      modals[0].insert("big");

      modals.insert(2, gum::Sequence< std::string >());
      modals[2].insert("big");
      modals[2].insert("bigbig");
      modals[2].insert("true");
      modals[2].insert("bigbigbig");
      modals[2].insert("false");

      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
      // GET_RESSOURCES_PATH("csv/asia3.csv"), modals, true);

      learner.useGreedyHillClimbing();
      learner.setMaxIndegree(10);
      learner.useScoreLog2Likelihood();

      TS_GUM_ASSERT_THROWS_NOTHING(learner.useScoreBDeu())
      learner.useScoreLog2Likelihood();

      learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});
      // learner.addForbiddenArc ( gum::Arc (4,3) );
      // learner.addForbiddenArc ( gum::Arc (5,1) );
      // learner.addForbiddenArc ( gum::Arc (5,7) );

      // learner.addMandatoryArc ( gum::Arc ( learner.nodeId ( "bronchitis" ),
      //                                      learner.nodeId ( "lung_cancer" )
      //                                      ) );

      learner.addMandatoryArc("bronchitis", "lung_cancer");

      learner.useSmoothingPrior();
      // learner.useDirichletPrior (  GET_RESSOURCES_PATH( "asia.csv" ) );

      gum::NodeProperty< gum::Size > slice_order{std::make_pair(gum::NodeId(0), (gum::Size)1),
                                                 std::make_pair(gum::NodeId(3), (gum::Size)0),
                                                 std::make_pair(gum::NodeId(1), (gum::Size)0)};
      learner.setSliceOrder(slice_order);

      const std::vector< std::string >& names = learner.names();
      TS_ASSERT(!names.empty())

      try {
        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.variable(0).domainSize(), (gum::Size)2)
        TS_ASSERT_EQUALS(bn.variable(2).domainSize(), (gum::Size)2)
        TS_ASSERT_EQUALS(bn.variable(0).label(0), "false")
        TS_ASSERT_EQUALS(bn.variable(0).label(1), "true")
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(_asia_with_user_modalities_string_incorrect) {
      auto bn = gum::BayesNet< double >::fastPrototype(
         "smoking{False|true|big};lung_cancer{big|bigbig|true|bigbigbig|false}");
      TS_ASSERT_THROWS(
         gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"), bn),
         gum::UnknownLabelInDatabase&)
    }

    GUM_ACTIVE_TEST(_asia_with_user_modalities_numbers) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia.csv"));
      // learner(GET_RESSOURCES_PATH("csv/asia.csv"), modals);
      learner.useGreedyHillClimbing();
      learner.setMaxIndegree(10);
      learner.useScoreLog2Likelihood();

      TS_GUM_ASSERT_THROWS_NOTHING(learner.useScoreBDeu())
      learner.useScoreLog2Likelihood();

      learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});
      learner.addForbiddenArc(gum::Arc(4, 3));
      learner.addForbiddenArc(gum::Arc(5, 1));
      learner.addForbiddenArc(gum::Arc(5, 7));

      learner.addMandatoryArc("bronchitis", "lung_cancer");

      learner.useSmoothingPrior();
      // learner.useDirichletPrior (  GET_RESSOURCES_PATH( "asia.csv" ) );

      gum::NodeProperty< gum::Size > slice_order{std::make_pair(gum::NodeId(0), (gum::Size)1),
                                                 std::make_pair(gum::NodeId(3), (gum::Size)0),
                                                 std::make_pair(gum::NodeId(1), (gum::Size)0)};
      learner.setSliceOrder(slice_order);

      const std::vector< std::string >& names = learner.names();
      TS_ASSERT(!names.empty())

      try {
        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.variable(0).domainSize(), (gum::Size)2)
        TS_ASSERT_EQUALS(bn.variable(2).domainSize(), (gum::Size)2)
        TS_ASSERT_EQUALS(bn.variable(0).label(0), "0")
        TS_ASSERT_EQUALS(bn.variable(0).label(1), "1")
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(test_asia_with_user_modalities_numbers_incorrect) {
      auto bn = gum::BayesNet< double >::fastPrototype(
         "smoking{1|2|big};lung_cancer{big|bigbig|3|bigbigbig|0}");
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia.csv")))
      //,                   gum::UnknownLabelInDatabase&)
    }

    GUM_ACTIVE_TEST(_asia_param) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));

      gum::DAG dag;

      for (unsigned int i = 0; i < 8; ++i) {
        dag.addNodeWithId(i);
      }

      for (unsigned int i = 0; i < 7; ++i) {
        dag.addArc(i, i + 1);
      }

      dag.addArc(0, 7);
      dag.addArc(2, 4);
      dag.addArc(5, 7);
      dag.addArc(3, 6);

      learner.useNoPrior();

      try {
        gum::BayesNet< double > bn = learner.learnParameters(dag);
        TS_ASSERT_EQUALS(bn.dim(), (gum::Size)25)
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(_asia_param_from_bn) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));

      learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});
      gum::BayesNet< double > bn = learner.learnBN();

      try {
        gum::BayesNet< double > bn2 = learner.learnParameters(bn.dag());
        TS_ASSERT_EQUALS(bn2.dag().arcs().size(), bn.dag().arcs().size())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(_asia_param_float) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));

      gum::DAG dag;

      for (unsigned int i = 0; i < 8; ++i) {
        dag.addNodeWithId(i);
      }

      for (unsigned int i = 0; i < 7; ++i) {
        dag.addArc(i, i + 1);
      }

      dag.addArc(0, 7);
      dag.addArc(2, 4);
      dag.addArc(5, 7);
      dag.addArc(3, 6);

      learner.useNoPrior();

      try {
        gum::BayesNet< double > bn = learner.learnParameters(dag);
        TS_ASSERT_EQUALS(bn.dim(), (gum::Size)25)
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(_asia_param_from_bn_float) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));

      learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});
      gum::BayesNet< double > bn = learner.learnBN();

      try {
        gum::BayesNet< double > bn2 = learner.learnParameters(bn.dag());
        TS_ASSERT_EQUALS(bn2.dag().arcs().size(), bn.dag().arcs().size())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(_asia_param_bn) {
      auto s = gum::fastVariable("smoking{false|true}");
      auto l = gum::fastVariable("lung_cancer{false|true}");
      auto b = gum::fastVariable("bronchitis{false|true}");
      auto v = gum::fastVariable("visit_to_Asia{false|true}");
      auto t = gum::fastVariable("tuberculosis{false|true}");
      auto o = gum::fastVariable("tuberculos_or_cancer{false|true}");
      auto d = gum::fastVariable("dyspnoea{false|true}");
      auto p = gum::fastVariable("positive_XraY{false|true}");

      gum::BayesNet< double > bn;
      gum::NodeId             ns = bn.add(*s);
      gum::NodeId             nl = bn.add(*l);
      gum::NodeId             nb = bn.add(*b);
      gum::NodeId             nv = bn.add(*v);
      gum::NodeId             nt = bn.add(*t);
      gum::NodeId             no = bn.add(*o);
      gum::NodeId             nd = bn.add(*d);
      gum::NodeId             np = bn.add(*p);

      bn.addArc(ns, nl);
      bn.addArc(ns, nb);
      bn.addArc(nl, no);
      bn.addArc(nb, nd);
      bn.addArc(nv, nt);
      bn.addArc(nt, no);
      bn.addArc(no, nd);
      bn.addArc(no, np);

      gum::learning::BNLearner learner(GET_RESSOURCES_PATH("csv/asia3.csv"), bn);

      learner.useScoreLog2Likelihood();
      learner.useSmoothingPrior();

      try {
        gum::BayesNet< double > bn2 = learner.learnParameters(bn.dag());
        TS_ASSERT_EQUALS(bn2.dim(), bn.dim())

        for (gum::NodeId node: bn.nodes()) {
          gum::NodeId node2 = bn2.idFromName(bn.variable(node).name());
          TS_ASSERT_EQUALS(bn.variable(node).toString(), bn2.variable(node2).toString())
        }

      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(_asia_param_bn_with_not_matching_variable) {
      auto s = gum::fastVariable("smoking{false|true}");
      auto l = gum::fastVariable("lung_cancer{false|true}");
      auto b = gum::fastVariable("bronchitis{false|true}");
      auto v = gum::fastVariable("visit_to_Asia{false|true}");
      auto t = gum::fastVariable("tuberculosis{false|true}");
      auto o = gum::fastVariable("tuberculos_or_cancer{false|true}");
      auto d = gum::fastVariable("dyspnoea{false|true}");

      // uncorrect name is : will it be correctly handled
      auto p = gum::fastVariable("ZORBLOBO{false|true}");

      gum::BayesNet< double > bn;
      bn.add(*s);
      bn.add(*l);
      bn.add(*b);
      bn.add(*v);
      bn.add(*t);
      bn.add(*o);
      bn.add(*d);
      bn.add(*p);

      TS_ASSERT_THROWS(
         gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"), bn),
         const gum::MissingVariableInDatabase&)


      // learner.useScoreLog2Likelihood();
      // learner.useSmoothingPrior();

      // TS_ASSERT_THROWS(gum::BayesNet< double > bn2 =
      // learner.learnParameters(bn),
      //                 const gum::MissingVariableInDatabase&);
    }

    GUM_ACTIVE_TEST(_asia_param_bn_with_subset_of_variables_in_base) {
      auto s = gum::fastVariable("smoking{false|true}");
      auto t = gum::fastVariable("tuberculosis{false|true}");
      auto o = gum::fastVariable("tuberculos_or_cancer{false|true}");
      auto d = gum::fastVariable("dyspnoea{false|true}");

      gum::BayesNet< double > bn;
      gum::NodeId             ns = bn.add(*s);
      gum::NodeId             nt = bn.add(*t);
      gum::NodeId             no = bn.add(*o);
      gum::NodeId             nd = bn.add(*d);

      bn.addArc(ns, nt);
      bn.addArc(nt, no);
      bn.addArc(no, nd);

      gum::learning::BNLearner learner(GET_RESSOURCES_PATH("csv/asia3.csv"), bn);


      learner.useScoreLog2Likelihood();
      learner.useSmoothingPrior();

      gum::BayesNet< double > bn2 = learner.learnParameters(bn.dag());
    }

    GUM_ACTIVE_TEST(_asia_param_bn_with_unknow_modality) {
      auto s = gum::fastVariable("smoking{false|true}");
      auto t = gum::fastVariable("tuberculosis{false|true}");
      auto o = gum::fastVariable("tuberculos_or_cancer{false|true}");
      auto d = gum::fastVariable("dyspnoea{false|true}");

      gum::BayesNet< double > bn;
      gum::NodeId             ns = bn.add(*s);
      gum::NodeId             nt = bn.add(*t);
      gum::NodeId             no = bn.add(*o);
      gum::NodeId             nd = bn.add(*d);

      bn.addArc(ns, nt);
      bn.addArc(nt, no);
      bn.addArc(no, nd);

      // asia3-faulty contains a label "beurk" for variable "smokin"
      TS_ASSERT_THROWS(
         gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3-faulty.csv"),
                                                    bn),
         const gum::UnknownLabelInDatabase&)
    }

    GUM_ACTIVE_TEST(_listener) {
      {
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia.csv"));
        aSimpleBNLeanerListener            listen(learner);

        learner.setVerbosity(true);
        learner.setMaxIndegree(10);
        learner.useScoreK2();
        learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});

        gum::BayesNet< double > bn = learner.learnBN();

        TS_ASSERT_EQUALS(listen.getNbr(), (gum::Size)2)
        TS_ASSERT_EQUALS(listen.getMess(), "stopped on request")
        TS_ASSERT_EQUALS(learner.messageApproximationScheme(), "stopped on request")
      }
      {
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia2.csv"));
        aSimpleBNLeanerListener            listen(learner);

        learner.setVerbosity(true);
        learner.setMaxIndegree(10);
        learner.useScoreK2();
        learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});

        gum::BayesNet< double > bn = learner.learnBN();

        TS_ASSERT_EQUALS(listen.getNbr(), (gum::Size)3)
        TS_ASSERT_EQUALS(listen.getMess(), "stopped on request")
        TS_ASSERT_EQUALS(learner.messageApproximationScheme(), "stopped on request")
      }
      {
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia.csv"));
        aSimpleBNLeanerListener            listen(learner);

        learner.setVerbosity(true);
        learner.setMaxIndegree(2);
        learner.useLocalSearchWithTabuList();

        gum::BayesNet< double > bn = learner.learnBN();

        TS_ASSERT_DELTA(listen.getNbr(), (gum::Size)15, 1)   // 75
        TS_ASSERT_EQUALS(listen.getMess(), "stopped on request")
        TS_ASSERT_EQUALS(learner.messageApproximationScheme(), "stopped on request")
      }
      {
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia.csv"));
        aSimpleBNLeanerListener            listen(learner);

        learner.setVerbosity(true);
        learner.setMaxIndegree(2);
        learner.useGreedyHillClimbing();

        gum::BayesNet< double > bn = learner.learnBN();

        TS_ASSERT_DELTA(listen.getNbr(), (gum::Size)3, 1)   // 2?
        TS_ASSERT_EQUALS(listen.getMess(), "stopped on request")
        TS_ASSERT_EQUALS(learner.messageApproximationScheme(), "stopped on request")
      }
    }

    GUM_ACTIVE_TEST(_DBNTonda) {
      gum::BayesNet< double > dbn;
      gum::NodeId             bf_0 = dbn.add(gum::LabelizedVariable("bf_0", "bf_0", 4));
      /*gum::NodeId bf_t =*/dbn.add(gum::LabelizedVariable("bf_t", "bf_t", 4));
      gum::NodeId c_0  = dbn.add(gum::LabelizedVariable("c_0", "c_0", 5));
      gum::NodeId c_t  = dbn.add(gum::LabelizedVariable("c_t", "c_t", 5));
      gum::NodeId h_0  = dbn.add(gum::LabelizedVariable("h_0", "h_0", 5));
      gum::NodeId h_t  = dbn.add(gum::LabelizedVariable("h_t", "h_t", 5));
      gum::NodeId tf_0 = dbn.add(gum::LabelizedVariable("tf_0", "tf_0", 5));
      /*gum::NodeId tf_t =*/dbn.add(gum::LabelizedVariable("tf_t", "tf_t", 5));
      gum::NodeId wl_0 = dbn.add(gum::LabelizedVariable("wl_0", "wl_0", 4));
      gum::NodeId wl_t = dbn.add(gum::LabelizedVariable("wl_t", "wl_t", 4));

      for (auto n: {c_t, h_t, wl_t}) {
        dbn.addArc(tf_0, n);
        dbn.addArc(bf_0, n);
      }
      dbn.addArc(c_0, c_t);
      dbn.addArc(h_0, h_t);
      dbn.addArc(wl_0, wl_t);

      gum::BayesNet< double > learn1;
      {
        // inductive learning leads to scrambled modalities
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/DBN_Tonda.csv"));
        learner.useScoreLog2Likelihood();
        learner.useSmoothingPrior(1.0);
        learn1 = learner.learnParameters(dbn.dag());
      }
      gum::BayesNet< double > learn2;
      {
        try {
          // while explicit learning does the right thing
          gum::learning::BNLearner learner(GET_RESSOURCES_PATH("csv/DBN_Tonda.csv"), learn1);
          learner.useScoreLog2Likelihood();
          learner.useSmoothingPrior(1.0);
          learn2 = learner.learnParameters(dbn.dag());
        } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
      }
      gum::BayesNet< double > learn3;
      {
        // while explicit learning does the right thing
        gum::learning::BNLearner learner(GET_RESSOURCES_PATH("csv/DBN_Tonda.csv"), dbn);
        learner.useScoreLog2Likelihood();
        learner.useSmoothingPrior(1.0);
        learn3 = learner.learnParameters(dbn.dag());
      }

      TS_ASSERT_EQUALS(learn1.variable(learn1.idFromName("wl_0")).toString(), "wl_0:Range([0,3])")
      TS_ASSERT_EQUALS(learn2.variable(learn2.idFromName("wl_0")).toString(), "wl_0:Range([0,3])")
      TS_ASSERT_EQUALS(learn2.variable(learn3.idFromName("wl_0")).toString(), "wl_0:Range([0,3])")

      auto&              p1 = learn1.cpt(learn1.idFromName("c_0"));
      auto&              p2 = learn2.cpt(learn2.idFromName("c_0"));
      auto&              p3 = learn3.cpt(learn3.idFromName("c_0"));
      gum::Instantiation I1(p1);
      gum::Instantiation I2(p2);
      gum::Instantiation I3(p3);

      for (I1.setFirst(), I2.setFirst(), I3.setFirst(); !I1.end(); I1.inc(), I2.inc(), I3.inc()) {
        TS_ASSERT_EQUALS(I1.toString(), I2.toString())   // same modalities orders
        TS_ASSERT_EQUALS(I1.toString(), I3.toString())   // same modalities orders
        TS_ASSERT_EQUALS(p1[I1], p2[I2])                 // same probabilities
        TS_ASSERT_EQUALS(p1[I1], p3[I3])                 // same probabilities
      }

      gum::BayesNet< double > learn4;
      {
        // inductive learning leads to scrambled modalities
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/DBN_Tonda.csv"),
                                                   {"?"},
                                                   false);
        learner.useScoreLog2Likelihood();
        learner.useSmoothingPrior(1.0);
        learn4 = learner.learnParameters(dbn.dag());
      }
      TS_ASSERT_EQUALS(learn4.variable(learn1.idFromName("wl_0")).toString(),
                       "wl_0:Labelized({0|1|2|3})")
    }

    GUM_ACTIVE_TEST(_asia_with_missing_values) {
      int nb = 0;
      try {
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3-faulty.csv"),
                                                   std::vector< std::string >{"BEURK"});
        learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});
        learner.learnBN();
      } catch (gum::MissingValueInDatabase&) { nb = 1; }

      TS_ASSERT_EQUALS(nb, 1)
    }

    GUM_ACTIVE_TEST(_BugDoumenc) {
      gum::BayesNet< double >    templ;
      std::vector< std::string > varBool{"S",
                                         "DEP",
                                         "TM",
                                         "TE",
                                         "TV",
                                         "PSY",
                                         "AL",
                                         "PT",
                                         "HYP",
                                         "FRE",
                                         "PC",
                                         "C",
                                         "MN",
                                         "AM",
                                         "PR",
                                         "AR",
                                         "DFM"};   // binary variables for the BN

      std::vector< std::string > varTer{"NBC",
                                        "MED",
                                        "DEM",
                                        "SP"};                // ternary variables for the bN

      std::vector< std::string > varContinuous{"A", "ADL"};   // continuous variables for the BN


      std::vector< gum::NodeId > nodeList;                    // nodes list for the BN

      for (const auto& var: varBool)
        nodeList.push_back(templ.add(gum::LabelizedVariable(var,
                                                            var,
                                                            2)));   // adding binary variables

      for (const auto& var: varTer)
        nodeList.push_back(templ.add(gum::LabelizedVariable(var,
                                                            var,
                                                            3)));   // adding ternary variables

      gum::DiscretizedVariable< double > A("A", "A");
      for (int i = 60; i <= 105; i += 5) {
        A.addTick(double(i));
      }

      gum::NodeId a_id = templ.add(A);
      nodeList.push_back(a_id);   // adding continuous variable

      // adding continuous variable
      nodeList.push_back(templ.add(gum::RangeVariable("ADL", "ADL", 0, 6)));
      // Création du noeud central NRC (niveau de risque de chute)
      gum::LabelizedVariable NRC("NRC", "NRC", 0);

      NRC.addLabel("faible");
      NRC.addLabel("modere");
      NRC.addLabel("eleve");
      auto iNRC = templ.add(NRC);

      // adding arcs from NRC
      for (auto node: nodeList) {
        templ.addArc(iNRC, node);
      }

      gum::learning::BNLearner learner(GET_RESSOURCES_PATH("csv/bugDoumenc.csv"), templ);
      learner.useScoreLog2Likelihood();
      learner.useSmoothingPrior();
      auto bn = learner.learnParameters(templ.dag());
    }

    GUM_ACTIVE_TEST(_BugDoumencWithInt) {
      gum::BayesNet< double >    templ;
      std::vector< std::string > varBool{"S",
                                         "DEP",
                                         "TM",
                                         "TE",
                                         "TV",
                                         "PSY",
                                         "AL",
                                         "PT",
                                         "HYP",
                                         "FRE",
                                         "PC",
                                         "C",
                                         "MN",
                                         "AM",
                                         "PR",
                                         "AR",
                                         "DFM"};   // les vraibles booléennes du RB

      std::vector< std::string > varTer{
         "NBC",
         "MED",
         "DEM",
         "SP"};   // les variables pouvant prendre 3 valeurs possibles du RB

      std::vector< std::string > varContinuous{"A", "ADL"};   // les variables continues du RB


      std::vector< gum::NodeId > nodeList;                    // Liste des noeuds du RB

      for (const auto& var: varBool)
        nodeList.push_back(templ.add(
           gum::LabelizedVariable(var,
                                  var,
                                  2)));   // Ajout des variables booléennes à la liste des noeuds

      for (const auto& var: varTer)
        nodeList.push_back(templ.add(
           gum::LabelizedVariable(var,
                                  var,
                                  3)));   // Ajout des variables ternaires à la liste des noeuds

      gum::DiscretizedVariable< int > A("A", "A");
      for (int i = 60; i <= 105; i += 5) {
        A.addTick(i);
      }

      nodeList.push_back(templ.add(A));   // Ajout de la variable Age allant de 60
      // à 100 ans à la liste des noeuds

      // Ajout de la variable ADL allant de 0 à 6 à la liste des noeuds
      nodeList.push_back(templ.add(gum::RangeVariable("ADL", "ADL", 0, 6)));
      // Création du noeud central NRC (niveau de risque de chute)
      gum::LabelizedVariable NRC("NRC", "NRC", 0);

      NRC.addLabel("faible");
      NRC.addLabel("modere");
      NRC.addLabel("eleve");
      auto iNRC = templ.add(NRC);

      // Création des arcs partant du noeud NRC vers les autres noeuds
      for (auto node: nodeList) {
        templ.addArc(iNRC, node);
      }


      gum::learning::BNLearner learner(GET_RESSOURCES_PATH("csv/bugDoumenc.csv"), templ);
      learner.useScoreLog2Likelihood();
      learner.useSmoothingPrior();

      auto bn = learner.learnParameters(templ.dag());

      const gum::DiscreteVariable& var_discr = bn.variable("A");
      int                          good      = 1;
      try {
        const auto& xvar_discr = dynamic_cast< const gum::DiscretizedVariable< int >& >(var_discr);
        TS_ASSERT_EQUALS(xvar_discr.domainSize(), (gum::Size)9)
        TS_ASSERT_EQUALS(xvar_discr.label(0), "[60;65[")
        TS_ASSERT_EQUALS(xvar_discr.label(1), "[65;70[")
        TS_ASSERT_EQUALS(xvar_discr.label(8), "[100;105]")
      } catch (const std::bad_cast&) { good = 0; }
      TS_ASSERT_EQUALS(good, 1)
    }

    GUM_ACTIVE_TEST(_setSliceOrderWithNames) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
      learner.setSliceOrder(
         {{"smoking", "lung_cancer"}, {"bronchitis", "visit_to_Asia"}, {"tuberculosis"}});


      gum::learning::BNLearner< double > learner2(GET_RESSOURCES_PATH("csv/asia3.csv"));
      TS_ASSERT_THROWS(learner2.setSliceOrder({{"smoking", "lung_cancer"},
                                               {"bronchitis", "visit_to_Asia"},
                                               {"smoking", "tuberculosis", "lung_cancer"}}),
                       const gum::DuplicateElement&)

      gum::learning::BNLearner< double > learner3(GET_RESSOURCES_PATH("csv/asia3.csv"));
      TS_ASSERT_THROWS(
         learner3.setSliceOrder(
            {{"smoking", "lung_cancer"}, {"bronchitis", "visit_to_Asia"}, {"CRUCRU"}}),
         const gum::MissingVariableInDatabase&)
    }

    GUM_ACTIVE_TEST(_dirichlet) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D->E<-B");

      gum::learning::BNDatabaseGenerator< double > genere(bn);
      genere.setRandomVarOrder();
      genere.drawSamples(2000);
      genere.toCSV(GET_RESSOURCES_PATH("outputs/bnlearner_dirichlet.csv"));

      auto bn2 = gum::BayesNet< double >::fastPrototype("A->B->C->D->E");
      gum::learning::BNDatabaseGenerator< double > genere2(bn2);
      genere2.drawSamples(100);
      genere2.toCSV(GET_RESSOURCES_PATH("outputs/bnlearner_database.csv"));

      gum::learning::BNLearner learner(GET_RESSOURCES_PATH("outputs/bnlearner_database.csv"), bn);
      learner.useDirichletPrior(GET_RESSOURCES_PATH("outputs/bnlearner_dirichlet.csv"), 10);
      learner.useScoreAIC();

      try {
        gum::BayesNet< double > bn3 = learner.learnBN();
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(_dirichlet2) {
      // read the learning database
      gum::learning::DBInitializerFromCSV initializer(
         GET_RESSOURCES_PATH("csv/db_dirichlet_learning.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet                translator_set;
      gum::learning::DBTranslator4LabelizedVariable translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);


      // read the prior database
      gum::learning::DBInitializerFromCSV dirichlet_initializer(
         GET_RESSOURCES_PATH("csv/db_dirichlet_prior.csv"));
      const auto&       dirichlet_var_names = initializer.variableNames();
      const std::size_t dirichlet_nb_vars   = dirichlet_var_names.size();

      gum::learning::DBTranslatorSet dirichlet_translator_set;
      for (std::size_t i = 0; i < dirichlet_nb_vars; ++i) {
        dirichlet_translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable dirichlet_database(dirichlet_translator_set);
      dirichlet_database.setVariableNames(dirichlet_initializer.variableNames());
      dirichlet_initializer.fillDatabase(dirichlet_database);
      dirichlet_database.reorder();


      // create the score and the prior
      gum::learning::DBRowGeneratorSet          dirichlet_genset;
      gum::learning::DBRowGeneratorParser       dirichlet_parser(dirichlet_database.handler(),
                                                           dirichlet_genset);
      gum::learning::DirichletPriorFromDatabase prior(dirichlet_database, dirichlet_parser);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);

      std::vector< double > weights{0, 1.0, 5.0, 10.0, 1000.0, 7000.0, 100000.0};

      gum::learning::BNLearner< double > learner(
         GET_RESSOURCES_PATH("csv/db_dirichlet_learning.csv"));
      learner.useGreedyHillClimbing();
      learner.useScoreBIC();

      for (const auto weight: weights) {
        prior.setWeight(weight);
        gum::learning::ScoreBIC score(parser, prior);

        // finalize the learning algorithm
        gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintDAG >
           struct_constraint;

        gum::learning::ParamEstimatorML estimator(parser, prior, score.internalPrior());

        gum::learning::GraphChangesGenerator4DiGraph op_set(struct_constraint);

        gum::learning::GraphChangesSelector4DiGraph selector(score, struct_constraint, op_set);

        gum::learning::GreedyHillClimbing search;

        gum::BayesNet< double > bn = search.learnBN(selector, estimator);


        learner.useDirichletPrior(GET_RESSOURCES_PATH("csv/db_dirichlet_prior.csv"), weight);

        gum::BayesNet< double > xbn = learner.learnBN();

        TS_ASSERT_EQUALS(xbn.moralGraph(), bn.moralGraph())
      }
    }

    GUM_ACTIVE_TEST(_EM) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/EM.csv"),
                                                 std::vector< std::string >{"?"});

      TS_ASSERT(learner.hasMissingValues())

      gum::DAG dag;
      for (auto i = std::size_t(0); i < learner.database().nbVariables(); ++i) {
        dag.addNodeWithId(gum::NodeId(i));
      }
      dag.addArc(gum::NodeId(1), gum::NodeId(0));
      dag.addArc(gum::NodeId(2), gum::NodeId(1));
      dag.addArc(gum::NodeId(3), gum::NodeId(2));

      TS_ASSERT_THROWS(learner.learnParameters(dag), const gum::MissingValueInDatabase&)

      learner.useEM(1e-3);
      learner.useSmoothingPrior();

      TS_GUM_ASSERT_THROWS_NOTHING(learner.learnParameters(dag, false))
      TS_GUM_ASSERT_THROWS_NOTHING(learner.nbrIterations())
    }

    GUM_ACTIVE_TEST(_chi2) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));

      auto reschi2 = learner.chi2("smoking", "lung_cancer");
      TS_ASSERT_DELTA(reschi2.first, 36.2256, 1e-4)
      TS_ASSERT_DELTA(reschi2.second, 0, 1e-4)

      reschi2 = learner.chi2("smoking", "visit_to_Asia");
      TS_ASSERT_DELTA(reschi2.first, 1.1257, 1e-4)
      TS_ASSERT_DELTA(reschi2.second, 0.2886, 1e-4)

      reschi2 = learner.chi2("lung_cancer", "tuberculosis");
      TS_ASSERT_DELTA(reschi2.first, 0.6297, 1e-4)
      TS_ASSERT_DELTA(reschi2.second, 0.4274, 1e-4)

      reschi2 = learner.chi2("lung_cancer", "tuberculosis", {"tuberculos_or_cancer"});
      TS_ASSERT_DELTA(reschi2.first, 58.0, 1e-4)
      TS_ASSERT_DELTA(reschi2.second, 0.0, 1e-4)

      // see IndepTestChi2TestSuite::test_statistics
      gum::learning::BNLearner< double > learner2(GET_RESSOURCES_PATH("csv/chi2.csv"));

      auto stat = learner2.chi2("A", "C");
      TS_ASSERT_DELTA(stat.first, 0.0007, 1e-3)
      TS_ASSERT_DELTA(stat.second, 0.978, 1e-3)

      stat = learner2.chi2("A", "B");
      TS_ASSERT_DELTA(stat.first, 21.4348, 1e-3)
      TS_ASSERT_DELTA(stat.second, 3.6e-6, TS_GUM_SMALL_ERROR)

      stat = learner2.chi2("B", "A");
      TS_ASSERT_DELTA(stat.first, 21.4348, 1e-3)
      TS_ASSERT_DELTA(stat.second, 3.6e-6, TS_GUM_SMALL_ERROR)

      stat = learner2.chi2("B", "D");
      TS_ASSERT_DELTA(stat.first, 0.903, 1e-3)
      TS_ASSERT_DELTA(stat.second, 0.341, 1e-3)

      stat = learner2.chi2("A", "C", {"B"});
      TS_ASSERT_DELTA(stat.first, 15.2205, 1e-3)
      TS_ASSERT_DELTA(stat.second, 0.0005, 1e-4)
    }

    GUM_ACTIVE_TEST(_G2) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
      auto                               resg2 = learner.G2("smoking", "lung_cancer");
      TS_ASSERT_DELTA(resg2.first, 43.0321, 1e-4)
      TS_ASSERT_DELTA(resg2.second, 0, 1e-4)

      resg2 = learner.G2("smoking", "visit_to_Asia");
      TS_ASSERT_DELTA(resg2.first, 1.1418, 1e-4)
      TS_ASSERT_DELTA(resg2.second, 0.2852, 1e-4)

      resg2 = learner.G2("lung_cancer", "tuberculosis");
      TS_ASSERT_DELTA(resg2.first, 1.2201, 1e-4)
      TS_ASSERT_DELTA(resg2.second, 0.2693, 1e-4)

      resg2 = learner.G2("lung_cancer", "tuberculosis", {"tuberculos_or_cancer"});
      TS_ASSERT_DELTA(resg2.first, 59.1386, 1e-4)
      TS_ASSERT_DELTA(resg2.second, 0.0, 1e-4)

      // see IndepTestChi2TestSuite::test_statistics
      gum::learning::BNLearner< double > learner2(GET_RESSOURCES_PATH("csv/chi2.csv"));

      auto stat = learner2.G2("A", "C");
      TS_ASSERT_DELTA(stat.first, 0.0007, 1e-3)
      TS_ASSERT_DELTA(stat.second, 0.978, 1e-4)

      stat = learner2.G2("A", "B");
      TS_ASSERT_DELTA(stat.first, 21.5846, 1e-3)
      TS_ASSERT_DELTA(stat.second, 3.6e-6, 1e-4)

      stat = learner2.G2("B", "A");
      TS_ASSERT_DELTA(stat.first, 21.5846, 1e-3)
      TS_ASSERT_DELTA(stat.second, 3.6e-6, 1e-4)

      stat = learner2.G2("B", "D");
      TS_ASSERT_DELTA(stat.first, 0.903, 1e-3)
      TS_ASSERT_DELTA(stat.second, 0.342, 1e-4)

      stat = learner2.G2("A", "C", {"B"});
      TS_ASSERT_DELTA(stat.first, 16.3470, 1e-3)
      TS_ASSERT_DELTA(stat.second, 0.0002, 1e-4)
    }

    GUM_ACTIVE_TEST(_cmpG2Chi2) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/testXYbase.csv"));
      auto [a0, b0] = learner.chi2("X", "Y");
      TS_ASSERT_DELTA(a0, 15.3389, 1e-3)
      TS_ASSERT_DELTA(b0, 0.01777843046460533, 1e-6)
      auto [a1, b1] = learner.G2("X", "Y");
      TS_ASSERT_DELTA(a1, 16.6066, 1e-3)
      TS_ASSERT_DELTA(b1, 0.0108433, 1e-6)
    }

    GUM_ACTIVE_TEST(_loglikelihood) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/chi2.csv"));
      TS_ASSERT_EQUALS(learner.nbRows(), (gum::Size)500)
      TS_ASSERT_EQUALS(learner.nbCols(), (gum::Size)4)

      double siz = -1.0 * learner.database().size();
      learner.useNoPrior();

      auto stat = learner.logLikelihood({"A"}) / siz;                  // LL=-N.H
      TS_ASSERT_DELTA(stat, 0.99943499, TS_GUM_SMALL_ERROR)
      stat = learner.logLikelihood({"B"}) / siz;                       // LL=-N.H
      TS_ASSERT_DELTA(stat, 0.9986032, TS_GUM_SMALL_ERROR)
      stat = learner.logLikelihood({std::string("A"), "B"}) / siz;     // LL=-N.H
      TS_ASSERT_DELTA(stat, 1.9668973, TS_GUM_SMALL_ERROR)
      stat = learner.logLikelihood({std::string("A")}, {"B"}) / siz;   // LL=-N.H
      TS_ASSERT_DELTA(stat, 1.9668973 - 0.9986032, TS_GUM_SMALL_ERROR)

      stat = learner.logLikelihood({"C"}) / siz;                       // LL=-N.H
      TS_ASSERT_DELTA(stat, 0.99860302, TS_GUM_SMALL_ERROR)
      stat = learner.logLikelihood({"D"}) / siz;                       // LL=-N.H
      TS_ASSERT_DELTA(stat, 0.40217919, TS_GUM_SMALL_ERROR)
      stat = learner.logLikelihood({std::string("C"), "D"}) / siz;     // LL=-N.H
      TS_ASSERT_DELTA(stat, 1.40077995, TS_GUM_SMALL_ERROR)
      stat = learner.logLikelihood({std::string("C")}, {"D"}) / siz;   // LL=-N.H
      TS_ASSERT_DELTA(stat, 1.40077995 - 0.40217919, TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(_errorFromPyagrum) {
      try {
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/sample_asia.csv"));
        learner.useMIIC();
        learner.useNMLCorrection();
        auto ge3off2 = learner.learnPDAG();
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(_PossibleEdgesMIIC) {
      //[smoking , lung_cancer , bronchitis , visit_to_Asia , tuberculosis ,
      // tuberculos_or_cancer , dyspnoea , positive_XraY]
      {
        // possible edges are not relevant
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
        learner.useMIIC();
        learner.addPossibleEdge("visit_to_Asia", "lung_cancer");
        learner.addPossibleEdge("visit_to_Asia", "smoking");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)0)
      }

      {
        // possible edges are relevant
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
        learner.useMIIC();
        learner.addPossibleEdge("smoking", "lung_cancer");
        learner.addPossibleEdge("bronchitis", "smoking");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_LESS_THAN_EQUALS(bn.sizeArcs(), (gum::Size)2);
        gum::ArcSet possible_arcs{gum::Arc(bn.idFromName("smoking"), bn.idFromName("lung_cancer")),
                                  gum::Arc(bn.idFromName("bronchitis"), bn.idFromName("smoking")),
                                  gum::Arc(bn.idFromName("smoking"), bn.idFromName("bronchitis")),
                                  gum::Arc(bn.idFromName("lung_cancer"), bn.idFromName("smoking"))};
        TS_ASSERT(possible_arcs.isSupersetOrEqual(bn.arcs()));
      }

      {
        // possible edges are relevant
        // mixed with a forbidden arcs
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
        learner.useMIIC();
        learner.addPossibleEdge("smoking", "lung_cancer");
        learner.addPossibleEdge("bronchitis", "smoking");
        learner.addForbiddenArc("smoking", "bronchitis");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_LESS_THAN_EQUALS(bn.sizeArcs(), (gum::Size)2);
        gum::ArcSet possible_arcs{gum::Arc(bn.idFromName("bronchitis"), bn.idFromName("smoking")),
                                  gum::Arc(bn.idFromName("smoking"), bn.idFromName("bronchitis")),
                                  gum::Arc(bn.idFromName("lung_cancer"), bn.idFromName("smoking"))};
        TS_ASSERT(possible_arcs.isSupersetOrEqual(bn.arcs()));
      }

      {
        // possible edges are relevant
        // mixed with a mandatory arcs
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
        learner.useMIIC();
        learner.addPossibleEdge("smoking", "lung_cancer");
        learner.addPossibleEdge("bronchitis", "smoking");
        learner.addMandatoryArc("visit_to_Asia", "bronchitis");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_LESS_THAN_EQUALS(bn.sizeArcs(), (gum::Size)3);
        TS_ASSERT_LESS_THAN_EQUALS((gum::Size)1, bn.sizeArcs());

        gum::ArcSet possible_arcs{
           gum::Arc(bn.idFromName("smoking"), bn.idFromName("lung_cancer")),
           gum::Arc(bn.idFromName("bronchitis"), bn.idFromName("smoking")),
           gum::Arc(bn.idFromName("smoking"), bn.idFromName("bronchitis")),
           gum::Arc(bn.idFromName("lung_cancer"), bn.idFromName("smoking")),
           gum::Arc(bn.idFromName("visit_to_Asia"), bn.idFromName("bronchitis"))};
        TS_ASSERT(possible_arcs.isSupersetOrEqual(bn.arcs()));
        TS_ASSERT(bn.arcs().contains(
           gum::Arc(bn.idFromName("visit_to_Asia"), bn.idFromName("bronchitis"))))
      }
    }

    GUM_ACTIVE_TEST(_PossibleEdgesGHC) {
      //[smoking , lung_cancer , bronchitis , visit_to_Asia , tuberculosis ,
      // tuberculos_or_cancer , dyspnoea , positive_XraY]
      {
        // possible edges are not relevant
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
        learner.useGreedyHillClimbing();
        learner.addPossibleEdge("visit_to_Asia", "lung_cancer");
        learner.addPossibleEdge("visit_to_Asia", "smoking");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)0)
      }

      {
        // possible edges are relevant
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
        learner.useGreedyHillClimbing();
        learner.addPossibleEdge("smoking", "lung_cancer");
        learner.addPossibleEdge("bronchitis", "smoking");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)2)
        TS_ASSERT(bn.parents("lung_cancer").contains(bn.idFromName("smoking")))
        TS_ASSERT(bn.parents("bronchitis").contains(bn.idFromName("smoking")))
      }

      {
        // possible edges are relevant
        // mixed with a forbidden arcs
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
        learner.useGreedyHillClimbing();
        learner.addPossibleEdge("smoking", "lung_cancer");
        learner.addPossibleEdge("bronchitis", "smoking");
        learner.addForbiddenArc("smoking", "bronchitis");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)2)
        TS_ASSERT(bn.parents("lung_cancer").contains(bn.idFromName("smoking")))
        TS_ASSERT(bn.parents("smoking").contains(bn.idFromName("bronchitis")))
      }

      {
        // possible edges are relevant
        // mixed with a mandatory arcs
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
        learner.useGreedyHillClimbing();
        learner.addPossibleEdge("smoking", "lung_cancer");
        learner.addPossibleEdge("bronchitis", "smoking");
        learner.addMandatoryArc("visit_to_Asia", "bronchitis");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)3)
        TS_ASSERT(bn.parents("lung_cancer").contains(bn.idFromName("smoking")))
        TS_ASSERT(bn.parents("smoking").contains(bn.idFromName("bronchitis")))
        TS_ASSERT(bn.parents("bronchitis").contains(bn.idFromName("visit_to_Asia")))
      }
    }

    GUM_ACTIVE_TEST(_PossibleEdgesTabu) {
      //[smoking , lung_cancer , bronchitis , visit_to_Asia , tuberculosis ,
      // tuberculos_or_cancer , dyspnoea , positive_XraY]
      {
        // possible edges are not relevant
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
        learner.useLocalSearchWithTabuList();
        learner.addPossibleEdge("visit_to_Asia", "lung_cancer");
        learner.addPossibleEdge("visit_to_Asia", "smoking");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)0)
      }

      {
        // possible edges are relevant
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
        learner.useLocalSearchWithTabuList();
        learner.addPossibleEdge("smoking", "lung_cancer");
        learner.addPossibleEdge("bronchitis", "smoking");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)2)
        TS_ASSERT(bn.parents("lung_cancer").contains(bn.idFromName("smoking")))
        TS_ASSERT(bn.parents("bronchitis").contains(bn.idFromName("smoking")))
      }

      {
        // possible edges are relevant
        // mixed with a forbidden arcs
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
        learner.useLocalSearchWithTabuList();
        learner.addPossibleEdge("smoking", "lung_cancer");
        learner.addPossibleEdge("bronchitis", "smoking");
        learner.addForbiddenArc("smoking", "bronchitis");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)2)
        TS_ASSERT(bn.parents("lung_cancer").contains(bn.idFromName("smoking")))
        TS_ASSERT(bn.parents("smoking").contains(bn.idFromName("bronchitis")))
      }

      {
        // possible edges are relevant
        // mixed with a mandatory arcs
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));
        learner.useLocalSearchWithTabuList();
        learner.addPossibleEdge("smoking", "lung_cancer");
        learner.addPossibleEdge("bronchitis", "smoking");
        learner.addMandatoryArc("visit_to_Asia", "bronchitis");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)3)
        TS_ASSERT(bn.parents("lung_cancer").contains(bn.idFromName("smoking")))
        TS_ASSERT(bn.parents("bronchitis").contains(bn.idFromName("smoking")))
        TS_ASSERT(bn.parents("bronchitis").contains(bn.idFromName("visit_to_Asia")))
      }
    }

    GUM_ACTIVE_TEST(PseudoCount) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/minimal.csv"));
      TS_ASSERT_EQUALS(learner.domainSize(0), 2u)
      TS_ASSERT_EQUALS(learner.domainSize("X"), 2u)
      TS_ASSERT_EQUALS(learner.domainSize(1), 2u)
      TS_ASSERT_EQUALS(learner.domainSize("Y"), 2u)
      TS_ASSERT_EQUALS(learner.domainSize(2), 3u)
      TS_ASSERT_EQUALS(learner.domainSize("Z"), 3u)
      learner.useNoPrior();

      TS_ASSERT_EQUALS(learner.rawPseudoCount(std::vector< gum::NodeId >({0})),
                       std::vector< double >({3, 4}))
      TS_ASSERT_EQUALS(learner.rawPseudoCount(std::vector< gum::NodeId >({0, 2})),
                       std::vector< double >({2, 1, 1, 1, 0, 2}))
      TS_ASSERT_EQUALS(learner.rawPseudoCount(std::vector< gum::NodeId >({2, 0})),
                       std::vector< double >({2, 1, 0, 1, 1, 2}))

      TS_ASSERT_EQUALS(learner.rawPseudoCount(std::vector< std::string >({"X"})),
                       std::vector< double >({3, 4}))
      TS_ASSERT_EQUALS(learner.rawPseudoCount(std::vector< std::string >({"X", "Z"})),
                       std::vector< double >({2, 1, 1, 1, 0, 2}))
      TS_ASSERT_EQUALS(learner.rawPseudoCount(std::vector< std::string >({"Z", "X"})),
                       std::vector< double >({2, 1, 0, 1, 1, 2}))
    }

    GUM_ACTIVE_TEST(NonRegressionZeroCount) {
      //////////////////////////
      // without specific score
      auto templ12 = gum::BayesNet< double >::fastPrototype("smoking->lung_cancer");
      gum::learning::BNLearner learner(GET_RESSOURCES_PATH("csv/asia.csv"), templ12);
      auto                     bn = learner.learnParameters(templ12.dag());

      gum::learning::BNLearner learner2(GET_RESSOURCES_PATH("csv/asia.csv"), templ12);
      auto                     bn2 = learner2.learnParameters(templ12.dag());
      TS_ASSERT_EQUALS(bn.cpt("lung_cancer").toString(), bn2.cpt("lung_cancer").toString())

      //////////////////////////
      // with score AIC
      auto templ34 = gum::BayesNet< double >::fastPrototype("smoking[3]->lung_cancer[3]");
      gum::learning::BNLearner learner3(GET_RESSOURCES_PATH("csv/asia.csv"), templ34);
      learner3.useScoreAIC();
      learner3.useSmoothingPrior(1e-6);

      auto bn3 = learner3.learnParameters(templ34.dag());
      {
        const gum::Potential< double >& p = bn.cpt("lung_cancer");
        const gum::Potential< double >& q = bn3.cpt("lung_cancer");

        auto I = gum::Instantiation(p);
        auto J = gum::Instantiation(q);

        TS_ASSERT_DELTA(p[I], q[J], 1e-6)
        ++I;
        ++J;
        TS_ASSERT_DELTA(p[I], q[J], 1e-6)
        ++J;
        TS_ASSERT_DELTA(0.0, q[J], 1e-6)
        ++I;
        ++J;
        TS_ASSERT_DELTA(p[I], q[J], 1e-6)
        ++I;
        ++J;
        TS_ASSERT_DELTA(p[I], q[J], 1e-6)
        ++J;
        TS_ASSERT_DELTA(0.0, q[J], 1e-6)
        ++J;
        TS_ASSERT_DELTA(1.0 / 3.0, q[J], 1e-6)
        ++J;
        TS_ASSERT_DELTA(1.0 / 3.0, q[J], 1e-6)
        ++J;
        TS_ASSERT_DELTA(1.0 / 3.0, q[J], 1e-6)
      }

      auto templ35 = gum::BayesNet< double >::fastPrototype("smoking[3]->lung_cancer[3]");
      gum::learning::BNLearner learner4(GET_RESSOURCES_PATH("csv/asia.csv"), templ35);
      learner4.useScoreAIC();

      TS_ASSERT_THROWS(learner4.learnParameters(templ34.dag()), const gum::DatabaseError&)
    }

    GUM_ACTIVE_TEST(_misorientation_MIIC) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/renewal.csv"));

      learner.useMIIC();
      learner.useNMLCorrection();

      auto bn            = learner.learnBN();
      auto expected_arcs = std::vector< std::pair< std::string, std::string > >(
         {{"coupon", "loyalty"},
          {"coupon", "recent visit"},
          {"loyalty", "renewal"},
          {"loyalty", "recent visit"},
          {"corporate customer", "loyalty"},
          {"corporate customer", "yearly consumption"},
          {"yearly consumption", "loyalty"},
          {"yearly consumption", "coupon"}});
      for (const auto& [tail, head]: expected_arcs) {
        TS_ASSERT(bn.existsArc(tail, head))
      }
    }

    GUM_ACTIVE_TEST(State) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/renewal.csv"));
      {
        auto state = learner.state();
        TS_ASSERT_EQUALS(state.size(), (gum::Size)9)
        TS_ASSERT_EQUALS(std::get< 0 >(state[0]), "Filename")
        TS_ASSERT_EQUALS(std::get< 1 >(state[0]), GET_RESSOURCES_PATH("csv/renewal.csv"))

        TS_ASSERT_EQUALS(std::get< 0 >(state[1]), "Size")
        TS_ASSERT_EQUALS(std::get< 1 >(state[1]), "(50000,6)")

        TS_ASSERT_EQUALS(std::get< 0 >(state[2]), "Variables")
        TS_ASSERT_EQUALS(std::get< 1 >(state[2]),
                         "loyalty[2], renewal[2], yearly consumption[5], corporate customer[2], "
                         "coupon[2], recent visit[2]")

        TS_ASSERT_EQUALS(std::get< 0 >(state[3]), "Induced types")
        TS_ASSERT_EQUALS(std::get< 1 >(state[3]), "True")

        TS_ASSERT_EQUALS(std::get< 0 >(state[4]), "Missing values")
        TS_ASSERT_EQUALS(std::get< 1 >(state[4]), "False")

        TS_ASSERT_EQUALS(std::get< 0 >(state[5]), "Algorithm")
        TS_ASSERT_EQUALS(std::get< 1 >(state[5]), "MIIC")

        TS_ASSERT_EQUALS(std::get< 0 >(state[6]), "Score")
        TS_ASSERT_EQUALS(std::get< 1 >(state[6]), "BDeu")

        TS_ASSERT_EQUALS(std::get< 0 >(state[7]), "Correction")
        TS_ASSERT_EQUALS(std::get< 1 >(state[7]), "MDL")

        TS_ASSERT_EQUALS(std::get< 0 >(state[8]), "Prior")
        TS_ASSERT_EQUALS(std::get< 1 >(state[8]), "-")
      }

      learner.useMIIC();
      learner.useNMLCorrection();

      {
        auto state = learner.state();
        TS_ASSERT_EQUALS(state.size(), (gum::Size)9)
        TS_ASSERT_EQUALS(std::get< 0 >(state[0]), "Filename")
        TS_ASSERT_EQUALS(std::get< 1 >(state[0]), GET_RESSOURCES_PATH("csv/renewal.csv"))

        TS_ASSERT_EQUALS(std::get< 0 >(state[1]), "Size")
        TS_ASSERT_EQUALS(std::get< 1 >(state[1]), "(50000,6)")

        TS_ASSERT_EQUALS(std::get< 0 >(state[2]), "Variables")
        TS_ASSERT_EQUALS(std::get< 1 >(state[2]),
                         "loyalty[2], renewal[2], yearly consumption[5], corporate customer[2], "
                         "coupon[2], recent visit[2]")


        TS_ASSERT_EQUALS(std::get< 0 >(state[3]), "Induced types")
        TS_ASSERT_EQUALS(std::get< 1 >(state[3]), "True")

        TS_ASSERT_EQUALS(std::get< 0 >(state[4]), "Missing values")
        TS_ASSERT_EQUALS(std::get< 1 >(state[4]), "False")

        TS_ASSERT_EQUALS(std::get< 0 >(state[5]), "Algorithm")
        TS_ASSERT_EQUALS(std::get< 1 >(state[5]), "MIIC")

        TS_ASSERT_EQUALS(std::get< 0 >(state[6]), "Score")
        TS_ASSERT_EQUALS(std::get< 1 >(state[6]), "BDeu")

        TS_ASSERT_EQUALS(std::get< 0 >(state[7]), "Correction")
        TS_ASSERT_EQUALS(std::get< 1 >(state[7]), "NML")

        TS_ASSERT_EQUALS(std::get< 0 >(state[8]), "Prior")
        TS_ASSERT_EQUALS(std::get< 1 >(state[8]), "-")
      }

      learner.addPossibleEdge("loyalty", "renewal");
      learner.setSliceOrder({{"loyalty", "renewal"}, {"recent visit", "corporate customer"}});
      {
        auto state = learner.state();
        TS_ASSERT_EQUALS(state.size(), (gum::Size)11)
        TS_ASSERT_EQUALS(std::get< 0 >(state[0]), "Filename")
        TS_ASSERT_EQUALS(std::get< 1 >(state[0]), GET_RESSOURCES_PATH("csv/renewal.csv"))

        TS_ASSERT_EQUALS(std::get< 0 >(state[1]), "Size")
        TS_ASSERT_EQUALS(std::get< 1 >(state[1]), "(50000,6)")

        TS_ASSERT_EQUALS(std::get< 0 >(state[2]), "Variables")
        TS_ASSERT_EQUALS(std::get< 1 >(state[2]),
                         "loyalty[2], renewal[2], yearly consumption[5], corporate customer[2], "
                         "coupon[2], recent visit[2]")


        TS_ASSERT_EQUALS(std::get< 0 >(state[3]), "Induced types")
        TS_ASSERT_EQUALS(std::get< 1 >(state[3]), "True")

        TS_ASSERT_EQUALS(std::get< 0 >(state[4]), "Missing values")
        TS_ASSERT_EQUALS(std::get< 1 >(state[4]), "False")

        TS_ASSERT_EQUALS(std::get< 0 >(state[5]), "Algorithm")
        TS_ASSERT_EQUALS(std::get< 1 >(state[5]), "MIIC")

        TS_ASSERT_EQUALS(std::get< 0 >(state[6]), "Score")
        TS_ASSERT_EQUALS(std::get< 1 >(state[6]), "BDeu")

        TS_ASSERT_EQUALS(std::get< 0 >(state[7]), "Correction")
        TS_ASSERT_EQUALS(std::get< 1 >(state[7]), "NML")

        TS_ASSERT_EQUALS(std::get< 0 >(state[8]), "Prior")
        TS_ASSERT_EQUALS(std::get< 1 >(state[8]), "-")

        TS_ASSERT_EQUALS(std::get< 0 >(state[9]), "Constraint Possible Edges")
        TS_ASSERT_EQUALS(std::get< 1 >(state[9]), "{loyalty--renewal}")

        TS_ASSERT_EQUALS(std::get< 0 >(state[10]), "Constraint Slice Order")
        TS_ASSERT_EQUALS(std::get< 1 >(state[10]),
                         "{corporate customer:1, renewal:0, loyalty:0, recent visit:1}")
      }

      gum::DAG dag;
      dag.addNodes(learner.nbCols());
      dag.addArc(0, 1);
      learner.setInitialDAG(dag);
      {
        auto state = learner.state();
        TS_ASSERT_EQUALS(state.size(), (gum::Size)12)
        TS_ASSERT_EQUALS(std::get< 0 >(state[11]), "Initial DAG")
        TS_ASSERT_EQUALS(std::get< 1 >(state[11]), "True")
      }
    }

    GUM_ACTIVE_TEST(StateContinued) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/renewal.csv"));
      learner.setDatabaseWeight(1000);
      learner.useK2(std::vector< gum::NodeId >{5, 4, 3, 2, 1, 0});
      {
        auto state = learner.state();
        TS_ASSERT_EQUALS(state.size(), (gum::Size)11)
        TS_ASSERT_EQUALS(std::get< 0 >(state[6]), "K2 order")
        TS_ASSERT_EQUALS(
           std::get< 1 >(state[6]),
           "recent visit, coupon, corporate customer, yearly consumption, renewal, loyalty")

        TS_ASSERT_EQUALS(std::get< 0 >(state[10]), "Database weight")
        TS_ASSERT_EQUALS(std::get< 1 >(state[10]), "1000.000000")
      }
      learner.useScoreAIC();
      learner.useBDeuPrior();
      {
        auto state = learner.state();
        TS_ASSERT_EQUALS(state.size(), (gum::Size)12)
        TS_ASSERT_EQUALS(std::get< 0 >(state[9]), "Prior")
        TS_ASSERT_DIFFERS(std::get< 2 >(state[9]), "")   // there is a comment about AIC versus BDeu
      }
    }

    GUM_ACTIVE_TEST(_multicore) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));

      learner.useGreedyHillClimbing();
      learner.useScoreBIC();
      learner.useSmoothingPrior();

      const std::size_t k        = 5;
      const auto&       database = learner.database();
      const std::size_t dbsize   = database.nbRows();
      std::size_t       foldSize = dbsize / k;

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::SmoothingPrior       prior(database);
      prior.setWeight(1);

      gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintDAG >
         struct_constraint;

      gum::learning::GraphChangesGenerator4DiGraph op_set(struct_constraint);

      gum::learning::GreedyHillClimbing search;

      gum::learning::ScoreBIC         score(parser, prior);
      gum::learning::ParamEstimatorML estimator(parser, prior, score.internalPrior());
      for (std::size_t fold = 0; fold < k; fold++) {
        // create the ranges of rows over which we perform the learning
        const std::size_t unfold_deb = fold * foldSize;
        const std::size_t unfold_end = unfold_deb + foldSize;

        std::vector< std::pair< std::size_t, std::size_t > > ranges;
        if (fold == std::size_t(0)) {
          ranges.push_back(std::pair< std::size_t, std::size_t >(unfold_end, dbsize));
        } else {
          ranges.push_back(std::pair< std::size_t, std::size_t >(std::size_t(0), unfold_deb));

          if (fold != k - 1) {
            ranges.push_back(std::pair< std::size_t, std::size_t >(unfold_end, dbsize));
          }
        }

        for (auto nb_threads = std::size_t(1); nb_threads < std::size_t(24); ++nb_threads) {
          learner.useDatabaseRanges(ranges);
          learner.setNumberOfThreads(nb_threads);

          TS_ASSERT_EQUALS(learner.databaseRanges(), ranges)

          learner.clearDatabaseRanges();
          TS_ASSERT_DIFFERS(learner.databaseRanges(), ranges)

          learner.useCrossValidationFold(fold, k);
          TS_ASSERT_EQUALS(learner.databaseRanges(), ranges)

          gum::BayesNet< double > bn1 = learner.learnBN();


          score.setRanges(ranges);
          estimator.setRanges(ranges);
          gum::learning::GraphChangesSelector4DiGraph selector(score, struct_constraint, op_set);
          gum::BayesNet< double > bn2 = search.learnBN< double >(selector, estimator);

          TS_ASSERT_EQUALS(bn1.dag(), bn2.dag())

          gum::Instantiation I1;
          gum::Instantiation I2;

          for (auto& name: database.variableNames()) {
            I1.add(bn1.variableFromName(name));
            I2.add(bn2.variableFromName(name));
          }

          double            LL1   = 0.0;
          double            LL2   = 0.0;
          const std::size_t nbCol = database.nbVariables();
          parser.setRange(unfold_deb, unfold_end);
          while (parser.hasRows()) {
            const gum::learning::DBRow< gum::learning::DBTranslatedValue >& row = parser.row();
            for (std::size_t i = 0; i < nbCol; ++i) {
              I1.chgVal(i, row[i].discr_val);
              I2.chgVal(i, row[i].discr_val);
            }

            LL1 += bn1.log2JointProbability(I1) * row.weight();
            LL2 += bn2.log2JointProbability(I2) * row.weight();
          }

          TS_ASSERT_EQUALS(LL1, LL2)
        }
      }
    }

    GUM_ACTIVE_TEST(MutualInformation) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));

      TS_ASSERT_DELTA(learner.mutualInformation("lung_cancer", "smoking"),
                      0.0332346154587978,
                      TS_GUM_SMALL_ERROR)

      learner.useNoCorrection();
      TS_ASSERT_DELTA(learner.mutualInformation("lung_cancer", "smoking"),
                      0.0332346154587978,
                      TS_GUM_SMALL_ERROR)

      learner.useMDLCorrection();
      TS_ASSERT_DELTA(learner.mutualInformation("lung_cancer", "smoking"),
                      0.0332346154587978,
                      TS_GUM_SMALL_ERROR)

      learner.useNMLCorrection();
      TS_ASSERT_DELTA(learner.mutualInformation("lung_cancer", "smoking"),
                      0.0332346154587978,
                      TS_GUM_SMALL_ERROR)

      learner.useNoCorrection();
      TS_ASSERT_DELTA(learner.correctedMutualInformation("lung_cancer", "smoking"),
                      0.0332346154587978,
                      TS_GUM_SMALL_ERROR)

      learner.useMDLCorrection();
      TS_ASSERT_DELTA(learner.correctedMutualInformation("lung_cancer", "smoking"),
                      0.0279523,
                      TS_GUM_SMALL_ERROR)

      learner.useNMLCorrection();
      TS_ASSERT_DELTA(learner.correctedMutualInformation("lung_cancer", "smoking"),
                      0.0292368,
                      TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(ConditionalMutualInformation) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/asia3.csv"));

      TS_ASSERT_DELTA(learner.mutualInformation("bronchitis", "lung_cancer", {"smoking"}),
                      0.00159046091867418,
                      TS_GUM_SMALL_ERROR)

      learner.useNoCorrection();
      TS_ASSERT_DELTA(learner.mutualInformation("bronchitis", "lung_cancer", {"smoking"}),
                      0.00159046091867418,
                      TS_GUM_SMALL_ERROR)

      learner.useMDLCorrection();
      TS_ASSERT_DELTA(learner.mutualInformation("bronchitis", "lung_cancer", {"smoking"}),
                      0.00159046091867418,
                      TS_GUM_SMALL_ERROR)

      learner.useNMLCorrection();
      TS_ASSERT_DELTA(learner.mutualInformation("bronchitis", "lung_cancer", {"smoking"}),
                      0.00159046091867418,
                      TS_GUM_SMALL_ERROR)

      learner.useNoCorrection();
      TS_ASSERT_DELTA(learner.correctedMutualInformation("bronchitis", "lung_cancer", {"smoking"}),
                      0.00159046091867418,
                      TS_GUM_SMALL_ERROR)

      learner.useMDLCorrection();
      TS_ASSERT_DELTA(learner.correctedMutualInformation("bronchitis", "lung_cancer", {"smoking"}),
                      -0.00897408,
                      TS_GUM_SMALL_ERROR)

      learner.useNMLCorrection();
      TS_ASSERT_DELTA(learner.correctedMutualInformation("bronchitis", "lung_cancer", {"smoking"}),
                      -0.00486096,
                      TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(learnParametersEM) {
      // default BN
      gum::BayesNet< double > bn;

      gum::LabelizedVariable b("b", "", 2), f("f", "", 2), w("w", "", 2), h("h", "", 2);
      auto                   bb = bn.add(b);
      auto                   ff = bn.add(f);
      auto                   ww = bn.add(w);
      auto                   hh = bn.add(h);
      bn.addArc(bb, ff);
      bn.addArc(bb, ww);
      bn.addArc(bb, hh);

      bn.cpt(bb).fillWith({0.5, 0.5});
      bn.cpt(ww).fillWith({0.8, 0.2, 0.3, 0.7});
      bn.cpt(ff).fillWith({0.8, 0.2, 0.3, 0.7});
      bn.cpt(hh).fillWith({0.2, 0.8, 0.7, 0.3});

      gum::learning::BNLearner learner(GET_RESSOURCES_PATH("csv/paramEM.csv"), bn);
      TS_ASSERT_EQUALS(learner.hasMissingValues(), true)

      learner.useEM(1e-2);
      learner.useSmoothingPrior();
      learner.setVerbosity(true);
      auto bn1 = learner.learnParameters(bn.dag());
      TS_ASSERT_LESS_THAN_EQUALS(1UL, learner.nbrIterations())

      for (auto node: bn.dag()) {
        const auto& cpt  = bn1.cpt(node);
        auto        cpt2 = cpt;
        cpt2.normalizeAsCPT();
        TS_ASSERT_EQUALS(cpt, cpt2)
      }
    }

    private:
    void _test_dirichlet(const gum::BayesNet< double >& model) {
      gum::learning::BNLearner all(GET_RESSOURCES_PATH("dirichlet/dirichlet.csv"), model);
      all.useNoPrior();
      auto full = all.learnParameters(model.dag(), true);

      const std::vector< std::string > parts = {GET_RESSOURCES_PATH("dirichlet/dir_part1.csv"),
                                                GET_RESSOURCES_PATH("dirichlet/dir_part2.csv"),
                                                GET_RESSOURCES_PATH("dirichlet/dir_part3.csv"),
                                                GET_RESSOURCES_PATH("dirichlet/dir_part4.csv")};

      gum::Size               nb_elt = 0;
      gum::BayesNet< double > partial;
      for (auto num_part = gum::Size(0); num_part < parts.size(); num_part++) {
        gum::learning::BNLearner learner(parts[num_part], model);
        if (num_part == 0) {   // first part
          learner.useNoPrior();
        } else {               // other parts, using partial(i-1) as prior
          learner.useDirichletPrior(partial, double(nb_elt));
        }
        partial = learner.learnParameters(model.dag(), true);
        nb_elt += learner.nbRows();
      }

      for (const auto& node: full.nodes()) {
        TS_GUM_POTENTIAL_DELTA(full.cpt(node),
                               gum::Potential(full.cpt(node)).fillWith(partial.cpt(node)),
                               TS_GUM_SMALL_ERROR)
      }
    }

    public:
    GUM_ACTIVE_TEST(_DirichletFromBN) {
      _test_dirichlet(gum::BayesNet< double >::fastPrototype("A;B;C;D", 3));
      _test_dirichlet(gum::BayesNet< double >::fastPrototype("A->B->C->D", 3));
      _test_dirichlet(gum::BayesNet< double >::fastPrototype("A<-B<-C<-D", 3));
      _test_dirichlet(gum::BayesNet< double >::fastPrototype("A<-B<-C->D", 3));
      _test_dirichlet(gum::BayesNet< double >::fastPrototype("A<-B->C->D", 3));
    }

    GUM_ACTIVE_TEST(NoCycleTest24) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/24.csv"));
      // used to raise a undirected cycle exception
      TS_GUM_ASSERT_THROWS_NOTHING(auto bn = learner.learnBN(););
    }

    GUM_ACTIVE_TEST(NoCycleTest125) {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("csv/bn125.csv"));
      TS_GUM_ASSERT_THROWS_NOTHING(auto bn = learner.learnBN(););
    }
  };   // class BNLearnerTestSuite
} /* namespace gum_tests */
