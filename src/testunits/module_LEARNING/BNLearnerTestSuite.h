
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


// floating point env
#include <cfenv>
#include <vector>
#include <string>
#include <iostream>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/learning/BNLearner.h>

#include <agrum/core/approximations/approximationSchemeListener.h>

#include <agrum/learning/database/BNDatabaseGenerator.h>

namespace gum_tests {

  class aSimpleBNLeanerListener : public gum::ApproximationSchemeListener {
    private:
    gum::Size   __nbr;
    std::string __mess;

    public:
    aSimpleBNLeanerListener(gum::IApproximationSchemeConfiguration& sch) :
        gum::ApproximationSchemeListener(sch), __nbr(0), __mess(""){};

    void whenProgress(const void*     buffer,
                      const gum::Size a,
                      const double    b,
                      const double    c) {
      __nbr++;
    }

    void whenStop(const void* buffer, const std::string s) { __mess = s; }

    gum::Size getNbr() { return __nbr; }

    std::string getMess() { return __mess; }
  };

  class BNLearnerTestSuite : public CxxTest::TestSuite {
    public:
    void test_asia() {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia3.csv"));

      learner.useLocalSearchWithTabuList(100, 1);
      learner.setMaxIndegree(10);
      learner.useScoreLog2Likelihood();

      TS_GUM_ASSERT_THROWS_NOTHING(learner.useScoreBD());
      TS_ASSERT_DIFFERS("", learner.checkScoreAprioriCompatibility());
      TS_GUM_ASSERT_THROWS_NOTHING(learner.useScoreBDeu());
      TS_ASSERT_EQUALS("", learner.checkScoreAprioriCompatibility());
      learner.useScoreLog2Likelihood();

      learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});
      // learner.addForbiddenArc ( gum::Arc (4,3) );
      // learner.addForbiddenArc ( gum::Arc (5,1) );
      // learner.addForbiddenArc ( gum::Arc (5,7) );

      // learner.addMandatoryArc ( gum::Arc ( learner.nodeId ( "bronchitis?" ),
      //                                      learner.nodeId ( "lung_cancer?" )
      //                                      ) );

      learner.addMandatoryArc("bronchitis?", "lung_cancer?");

      learner.useAprioriSmoothing();
      // learner.useAprioriDirichlet (  GET_RESSOURCES_PATH( "asia.csv" ) );

      gum::NodeProperty< gum::Size > slice_order{
         std::make_pair(gum::NodeId(0), gum::Size(1)),
         std::make_pair(gum::NodeId(3), gum::Size(0)),
         std::make_pair(gum::NodeId(1), gum::Size(0))};
      learner.setSliceOrder(slice_order);

      const std::vector< std::string >& names = learner.names();
      TS_ASSERT(!names.empty());

      try {
        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT(bn.dag().arcs().size() == 9);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }

      learner.setDatabaseWeight(10.0);
      const auto&  db = learner.database();
      const double weight = 10.0 / double(db.nbRows());
      for (const auto& row : db) {
        TS_ASSERT(row.weight() == weight);
      }
      TS_ASSERT_DELTA(learner.databaseWeight(), 10.0, 1e-4);

      const std::size_t nbr = db.nbRows();
      for (std::size_t i = std::size_t(0); i < nbr; ++i) {
        if (i % 2) learner.setRecordWeight(i, 2.0);
      }

      std::size_t index = std::size_t(0);
      for (const auto& row : db) {
        if (index % 2) {
          TS_ASSERT(row.weight() == 2.0);
          TS_ASSERT(learner.recordWeight(index) == 2.0);
        } else {
          TS_ASSERT(row.weight() == 10.0);
          TS_ASSERT(learner.recordWeight(index) == 10.0);
        }
        ++index;
      }
    }


    void test_ranges() {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia3.csv"));

      learner.useGreedyHillClimbing();
      learner.useScoreBIC();
      learner.useAprioriSmoothing();

      const std::size_t k = 5;
      const auto&       database = learner.database();
      const std::size_t dbsize = database.nbRows();
      std::size_t       foldSize = dbsize / k;

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
      gum::learning::AprioriSmoothing<>     apriori(database);
      apriori.setWeight(1);

      gum::learning::StructuralConstraintSetStatic<
         gum::learning::StructuralConstraintDAG >
         struct_constraint;

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
         op_set(struct_constraint);

      gum::learning::GreedyHillClimbing search;

      gum::learning::ScoreBIC<>         score(parser, apriori);
      gum::learning::ParamEstimatorML<> estimator(
         parser, apriori, score.internalApriori());
      for (std::size_t fold = 0; fold < k; fold++) {
        // create the ranges of rows over which we perform the learning
        const std::size_t unfold_deb = fold * foldSize;
        const std::size_t unfold_end = unfold_deb + foldSize;

        std::vector< std::pair< std::size_t, std::size_t > > ranges;
        if (fold == std::size_t(0)) {
          ranges.push_back(
             std::pair< std::size_t, std::size_t >(unfold_end, dbsize));
        } else {
          ranges.push_back(
             std::pair< std::size_t, std::size_t >(std::size_t(0), unfold_deb));

          if (fold != k - 1) {
            ranges.push_back(
               std::pair< std::size_t, std::size_t >(unfold_end, dbsize));
          }
        }

        learner.useDatabaseRanges(ranges);
        TS_ASSERT(learner.databaseRanges() == ranges);

        learner.clearDatabaseRanges();
        TS_ASSERT(learner.databaseRanges() != ranges);

        learner.useCrossValidationFold(fold, k);
        TS_ASSERT(learner.databaseRanges() == ranges);

        gum::BayesNet< double > bn1 = learner.learnBN();


        score.setRanges(ranges);
        estimator.setRanges(ranges);
        gum::learning::GraphChangesSelector4DiGraph< decltype(struct_constraint),
                                                     decltype(op_set) >
                                selector(score, struct_constraint, op_set);
        gum::BayesNet< double > bn2 =
           search.learnBN< double >(selector, estimator);

        TS_ASSERT(bn1.dag() == bn2.dag());

        gum::Instantiation I1, I2;

        for (auto& name : database.variableNames()) {
          I1.add(bn1.variableFromName(name));
          I2.add(bn2.variableFromName(name));
        }

        double            LL1 = 0.0, LL2 = 0.0;
        const std::size_t nbCol = database.nbVariables();
        parser.setRange(unfold_deb, unfold_end);
        while (parser.hasRows()) {
          const gum::learning::DBRow< gum::learning::DBTranslatedValue >& row =
             parser.row();
          for (std::size_t i = 0; i < nbCol; ++i) {
            I1.chgVal(i, row[i].discr_val);
            I2.chgVal(i, row[i].discr_val);
          }

          LL1 += bn1.log2JointProbability(I1) * row.weight();
          LL2 += bn2.log2JointProbability(I2) * row.weight();
        }

        TS_ASSERT(LL1 == LL2);
      }
    }


    void test_asia_3off2() {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia.csv"));

      aSimpleBNLeanerListener listen(learner);

      learner.useGreedyHillClimbing();
      TS_ASSERT_THROWS(learner.useNML(), gum::OperationNotAllowed);

      learner.use3off2();
      learner.useNML();
      learner.addForbiddenArc(gum::Arc(4, 1));
      // learner.addForbiddenArc ( gum::Arc (5,1) );
      // learner.addForbiddenArc ( gum::Arc (5,7) );

      learner.addMandatoryArc(gum::Arc(7, 5));
      gum::DAG i_dag;
      for (gum::NodeId i = 0; i < 8; ++i) {
        i_dag.addNodeWithId(i);
      }
      learner.setInitialDAG(i_dag);
      // learner.addMandatoryArc( "bronchitis?", "lung_cancer?" );

      const std::vector< std::string >& names = learner.names();
      TS_ASSERT(!names.empty());

      try {
        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.dag().arcs().size(), gum::Size(9));
        // TS_ASSERT_EQUALS(listen.getNbr(), gum::Size(86));
        TS_ASSERT(!bn.dag().existsArc(4, 1));
        TS_ASSERT(bn.dag().existsArc(7, 5));

        gum::MixedGraph mg = learner.learnMixedStructure();
        TS_ASSERT_EQUALS(mg.arcs().size(), gum::Size(8));
        TS_ASSERT_EQUALS(mg.edges().size(), gum::Size(1));
        TS_ASSERT(!mg.existsArc(4, 1));
        TS_ASSERT(mg.existsArc(7, 5));
        std::vector< gum::Arc > latents = learner.latentVariables();
        TS_ASSERT_EQUALS(latents.size(), gum::Size(2));
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    // WARNING: this test is commented on purpouse: you need a running database
    // with a table filled with the content of the asia.csv file. You will also
    // need a proper odbc configuration (under linux and macos you'll need
    // unixodbc and specific database odbc drivers).
    // void /*test*/_asia_db() {
    //   try {
    //     auto db = gum::learning::DatabaseFromSQL(
    //         "PostgreSQL",
    //         "lto",
    //         "Password2Change",
    //         "select smoking , lung_cancer , bronchitis , visit_to_asia , "
    //         "tuberculosis , tuberculos_or_cancer , dyspnoea , positive_xray "
    //         "from asia;" );
    //     gum::learning::BNLearner<double> learner( db );

    //     learner.useLocalSearchWithTabuList( 100, 1 );
    //     learner.setMaxIndegree( 10 );
    //     learner.useScoreLog2Likelihood();

    //     TS_ASSERT_THROWS( learner.useScoreBD(), gum::IncompatibleScoreApriori
    //     );
    //     TS_GUM_ASSERT_THROWS_NOTHING( learner.useScoreBDeu() );
    //     learner.useScoreLog2Likelihood();

    //     learner.useK2( std::vector<gum::NodeId>{1, 5, 2, 6, 0, 3, 4, 7} );
    //     learner.addMandatoryArc( "bronchitis", "lung_cancer" );
    //     learner.useAprioriSmoothing();

    //     gum::NodeProperty<unsigned int> slice_order{
    //         std::make_pair( gum::NodeId( 0 ), 1 ),
    //         std::make_pair( gum::NodeId( 3 ), 0 ),
    //         std::make_pair( gum::NodeId( 1 ), 0 )};

    //     const std::vector<std::string>& names = learner.names();
    //     TS_ASSERT( !names.empty() );

    //     try {
    //       gum::BayesNet<double> bn = learner.learnBN();
    //       TS_ASSERT( bn.dag().arcs().size() == 9 );
    //     } catch ( gum::Exception& e ) {
    //       GUM_SHOWERROR( e );
    //     }
    //   } catch ( gum::Exception& e ) {
    //     GUM_TRACE( e.errorType() );
    //     GUM_TRACE( e.errorContent() );
    //     GUM_TRACE( e.errorCallStack() );
    //     TS_FAIL("plop");
    //   }
    // }

    void test_asia_with_domain_sizes() {
      gum::learning::BNLearner< double > learn(GET_RESSOURCES_PATH("asia3.csv"));
      const auto&                        database = learn.database();

      gum::BayesNet< double > bn;
      for (auto& name : database.variableNames()) {
        gum::LabelizedVariable var(name, name, {"false", "true", "big"});
        bn.add(var);
      }

      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia3.csv"),
                                                 bn);
      learner.useScoreBIC();
      learner.useAprioriSmoothing();

      gum::BayesNet< double > bn2 = learner.learnBN();
      for (auto& name : database.variableNames()) {
        TS_ASSERT(bn2.variableFromName(name).domainSize() == 3);
      }
    }

    void xtest_asia_with_user_modalities_string_min() {
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

      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia3.csv"));
      // GET_RESSOURCES_PATH("asia3.csv"), modals, true);

      learner.useGreedyHillClimbing();
      learner.setMaxIndegree(10);
      learner.useScoreLog2Likelihood();

      TS_ASSERT_THROWS(learner.useScoreBD(), gum::IncompatibleScoreApriori);
      TS_GUM_ASSERT_THROWS_NOTHING(learner.useScoreBDeu());
      learner.useScoreLog2Likelihood();

      learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});
      // learner.addForbiddenArc ( gum::Arc (4,3) );
      // learner.addForbiddenArc ( gum::Arc (5,1) );
      // learner.addForbiddenArc ( gum::Arc (5,7) );

      // learner.addMandatoryArc ( gum::Arc ( learner.nodeId ( "bronchitis?" ),
      //                                      learner.nodeId ( "lung_cancer?" )
      //                                      ) );

      learner.addMandatoryArc("bronchitis?", "lung_cancer?");

      learner.useAprioriSmoothing();
      // learner.useAprioriDirichlet (  GET_RESSOURCES_PATH( "asia.csv" ) );

      gum::NodeProperty< gum::Size > slice_order{
         std::make_pair(gum::NodeId(0), gum::Size(1)),
         std::make_pair(gum::NodeId(3), gum::Size(0)),
         std::make_pair(gum::NodeId(1), gum::Size(0))};
      learner.setSliceOrder(slice_order);

      const std::vector< std::string >& names = learner.names();
      TS_ASSERT(!names.empty());

      try {
        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT(bn.variable(0).domainSize() == 2);
        TS_ASSERT(bn.variable(2).domainSize() == 2);
        TS_ASSERT(bn.variable(0).label(0) == "false");
        TS_ASSERT(bn.variable(0).label(1) == "true");
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    void xtest_asia_with_user_modalities_string_incorrect() {
      gum::NodeProperty< gum::Sequence< std::string > > modals;
      modals.insert(0, gum::Sequence< std::string >());
      modals[0].insert("False");
      modals[0].insert("true");
      modals[0].insert("big");

      modals.insert(2, gum::Sequence< std::string >());
      modals[2].insert("big");
      modals[2].insert("bigbig");
      modals[2].insert("true");
      modals[2].insert("bigbigbig");
      modals[2].insert("false");

      bool except = false;

      try {
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia3.csv"));
        // GET_RESSOURCES_PATH("asia3.csv"), modals);
        learner.useAprioriSmoothing();
      } catch (gum::UnknownLabelInDatabase&) { except = true; }

      TS_ASSERT(except);
    }

    void xtest_asia_with_user_modalities_numbers() {
      gum::NodeProperty< gum::Sequence< std::string > > modals;
      modals.insert(0, gum::Sequence< std::string >());
      modals[0].insert("0");
      modals[0].insert("1");
      modals[0].insert("big");

      modals.insert(2, gum::Sequence< std::string >());
      modals[2].insert("big");
      modals[2].insert("bigbig");
      modals[2].insert("1");
      modals[2].insert("bigbigbig");
      modals[2].insert("0");

      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia.csv"));
      // learner(GET_RESSOURCES_PATH("asia.csv"), modals);
      learner.useGreedyHillClimbing();
      learner.setMaxIndegree(10);
      learner.useScoreLog2Likelihood();

      TS_ASSERT_THROWS(learner.useScoreBD(), gum::IncompatibleScoreApriori);
      TS_GUM_ASSERT_THROWS_NOTHING(learner.useScoreBDeu());
      learner.useScoreLog2Likelihood();

      learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});
      learner.addForbiddenArc(gum::Arc(4, 3));
      learner.addForbiddenArc(gum::Arc(5, 1));
      learner.addForbiddenArc(gum::Arc(5, 7));

      learner.addMandatoryArc("bronchitis?", "lung_cancer?");

      learner.useAprioriSmoothing();
      // learner.useAprioriDirichlet (  GET_RESSOURCES_PATH( "asia.csv" ) );

      gum::NodeProperty< gum::Size > slice_order{
         std::make_pair(gum::NodeId(0), gum::Size(1)),
         std::make_pair(gum::NodeId(3), gum::Size(0)),
         std::make_pair(gum::NodeId(1), gum::Size(0))};
      learner.setSliceOrder(slice_order);

      const std::vector< std::string >& names = learner.names();
      TS_ASSERT(!names.empty());

      try {
        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT(bn.variable(0).domainSize() == 3);
        TS_ASSERT(bn.variable(2).domainSize() == 5);
        TS_ASSERT(bn.variable(0).label(0) == "0");
        TS_ASSERT(bn.variable(0).label(1) == "1");
        TS_ASSERT(bn.variable(0).label(2) == "big");
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    void xtest_asia_with_user_modalities_numbers_incorrect() {
      gum::NodeProperty< gum::Sequence< std::string > > modals;
      modals.insert(0, gum::Sequence< std::string >());
      modals[0].insert("1");
      modals[0].insert("2");
      modals[0].insert("big");

      modals.insert(2, gum::Sequence< std::string >());
      modals[2].insert("big");
      modals[2].insert("bigbig");
      modals[2].insert("3");
      modals[2].insert("bigbigbig");
      modals[2].insert("0");

      bool except = false;

      try {
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia.csv"));
        // learner(GET_RESSOURCES_PATH("asia.csv"), modals);
        learner.useAprioriSmoothing();
      } catch (gum::UnknownLabelInDatabase&) { except = true; }

      TS_ASSERT(except);
    }

    void test_asia_param() {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia3.csv"));

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

      learner.useNoApriori();

      try {
        gum::BayesNet< double > bn = learner.learnParameters(dag);
        TS_ASSERT(bn.dim() == 25);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    void test_asia_param_from_bn() {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia3.csv"));

      learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});
      gum::BayesNet< double > bn = learner.learnBN();

      try {
        gum::BayesNet< double > bn2 = learner.learnParameters(bn.dag());
        TS_ASSERT(bn2.dag().arcs().size() == bn.dag().arcs().size());
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    void test_asia_param_float() {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia3.csv"));

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

      learner.useNoApriori();

      try {
        gum::BayesNet< double > bn = learner.learnParameters(dag);
        TS_ASSERT(bn.dim() == 25);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    void test_asia_param_from_bn_float() {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia3.csv"));

      learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});
      gum::BayesNet< double > bn = learner.learnBN();

      try {
        gum::BayesNet< double > bn2 = learner.learnParameters(bn.dag());
        TS_ASSERT(bn2.dag().arcs().size() == bn.dag().arcs().size());
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    void test_asia_param_bn() {
#define createBoolVar(s) \
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

      gum::BayesNet< double > bn;
      gum::NodeId             ns = bn.add(s);
      gum::NodeId             nl = bn.add(l);
      gum::NodeId             nb = bn.add(b);
      gum::NodeId             nv = bn.add(v);
      gum::NodeId             nt = bn.add(t);
      gum::NodeId             no = bn.add(o);
      gum::NodeId             nd = bn.add(d);
      gum::NodeId             np = bn.add(p);

      bn.addArc(ns, nl);
      bn.addArc(ns, nb);
      bn.addArc(nl, no);
      bn.addArc(nb, nd);
      bn.addArc(nv, nt);
      bn.addArc(nt, no);
      bn.addArc(no, nd);
      bn.addArc(no, np);

      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia3.csv"),
                                                 bn);

      learner.useScoreLog2Likelihood();
      learner.useAprioriSmoothing();

      try {
        gum::BayesNet< double > bn2 = learner.learnParameters(bn.dag());
        TS_ASSERT(bn2.dim() == bn.dim());

        for (gum::NodeId node : bn.nodes()) {
          gum::NodeId node2 = bn2.idFromName(bn.variable(node).name());
          TS_ASSERT_EQUALS(bn.variable(node).toString(),
                           bn2.variable(node2).toString());
        }

      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    void test_asia_param_bn_with_not_matching_variable() {
#define createBoolVar(s) \
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

      gum::BayesNet< double > bn;
      bn.add(s);
      bn.add(l);
      bn.add(b);
      bn.add(v);
      bn.add(t);
      bn.add(o);
      bn.add(d);
      bn.add(p);

      TS_ASSERT_THROWS(gum::learning::BNLearner< double > learner(
                          GET_RESSOURCES_PATH("asia3.csv"), bn),
                       gum::MissingVariableInDatabase);


      // learner.useScoreLog2Likelihood();
      // learner.useAprioriSmoothing();

      // TS_ASSERT_THROWS(gum::BayesNet< double > bn2 =
      // learner.learnParameters(bn),
      //                 gum::MissingVariableInDatabase);
    }

    void test_asia_param_bn_with_subset_of_variables_in_base() {
#define createBoolVar(s) \
  gum::LabelizedVariable(s, s, 0).addLabel("false").addLabel("true");
      auto s = createBoolVar("smoking?");
      auto t = createBoolVar("tuberculosis?");
      auto o = createBoolVar("tuberculos_or_cancer?");
      auto d = createBoolVar("dyspnoea?");
#undef createBoolVar

      gum::BayesNet< double > bn;
      gum::NodeId             ns = bn.add(s);
      gum::NodeId             nt = bn.add(t);
      gum::NodeId             no = bn.add(o);
      gum::NodeId             nd = bn.add(d);

      bn.addArc(ns, nt);
      bn.addArc(nt, no);
      bn.addArc(no, nd);

      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia3.csv"),
                                                 bn);


      learner.useScoreLog2Likelihood();
      learner.useAprioriSmoothing();

      gum::BayesNet< double > bn2 = learner.learnParameters(bn.dag());
    }

    void test_asia_param_bn_with_unknow_modality() {
#define createBoolVar(s) \
  gum::LabelizedVariable(s, s, 0).addLabel("false").addLabel("true");
      auto s = createBoolVar("smoking?");
      auto t = createBoolVar("tuberculosis?");
      auto o = createBoolVar("tuberculos_or_cancer?");
      auto d = createBoolVar("dyspnoea?");
#undef createBoolVar

      gum::BayesNet< double > bn;
      gum::NodeId             ns = bn.add(s);
      gum::NodeId             nt = bn.add(t);
      gum::NodeId             no = bn.add(o);
      gum::NodeId             nd = bn.add(d);

      bn.addArc(ns, nt);
      bn.addArc(nt, no);
      bn.addArc(no, nd);

      // asia3-faulty contains a label "beurk" for variable "smoking?"
      // std::cout << "error test";

      TS_ASSERT_THROWS(gum::learning::BNLearner< double > learner(
                          GET_RESSOURCES_PATH("asia3-faulty.csv"), bn),
                       gum::UnknownLabelInDatabase);
    }

    void test_listener() {
      {
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia.csv"));
        aSimpleBNLeanerListener listen(learner);

        learner.setVerbosity(true);
        learner.setMaxIndegree(10);
        learner.useScoreK2();
        learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});

        gum::BayesNet< double > bn = learner.learnBN();

        TS_ASSERT_EQUALS(listen.getNbr(), gum::Size(2));
        TS_ASSERT_EQUALS(listen.getMess(), "stopped on request");
        TS_ASSERT_EQUALS(learner.messageApproximationScheme(),
                         "stopped on request");
      }
      {
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia2.csv"));
        aSimpleBNLeanerListener listen(learner);

        learner.setVerbosity(true);
        learner.setMaxIndegree(10);
        learner.useScoreK2();
        learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});

        gum::BayesNet< double > bn = learner.learnBN();

        TS_ASSERT_EQUALS(listen.getNbr(), gum::Size(3));
        TS_ASSERT_EQUALS(listen.getMess(), "stopped on request");
        TS_ASSERT_EQUALS(learner.messageApproximationScheme(),
                         "stopped on request");
      }
      {
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia.csv"));
        aSimpleBNLeanerListener listen(learner);

        learner.setVerbosity(true);
        learner.setMaxIndegree(2);
        learner.useLocalSearchWithTabuList();

        gum::BayesNet< double > bn = learner.learnBN();
        // std::cout << bn.dag () << std::endl;

        TS_ASSERT_DELTA(listen.getNbr(), gum::Size(15), 1);   // 75 ?
        TS_ASSERT_EQUALS(listen.getMess(), "stopped on request");
        TS_ASSERT_EQUALS(learner.messageApproximationScheme(),
                         "stopped on request");
      }
      {
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia.csv"));
        aSimpleBNLeanerListener listen(learner);

        learner.setVerbosity(true);
        learner.setMaxIndegree(2);
        learner.useGreedyHillClimbing();

        gum::BayesNet< double > bn = learner.learnBN();

        TS_ASSERT_DELTA(listen.getNbr(), gum::Size(3), 1);   // 2?
        TS_ASSERT_EQUALS(listen.getMess(), "stopped on request");
        TS_ASSERT_EQUALS(learner.messageApproximationScheme(),
                         "stopped on request");
      }
    }

    void test_DBNTonda() {
      gum::BayesNet< double > dbn;
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

      gum::BayesNet< double > learn1;
      {
        // inductive learning leads to scrambled modalities
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("DBN_Tonda.csv"));
        learner.useScoreLog2Likelihood();
        learner.useAprioriSmoothing(1.0);
        learn1 = learner.learnParameters(dbn.dag());
      }
      gum::BayesNet< double > learn2;
      {
        try {
          /*
          gum::NodeProperty< gum::Sequence< std::string > > modals;
          auto ds = std::vector< unsigned int >({4, 4, 5, 5, 5, 5, 5, 5, 4, 4});
          auto labels = std::vector< std::string >({"0", "1", "2", "3", "4", "5"});

          for (auto i = 0U; i < ds.size(); i++) {
            modals.insert(i, gum::Sequence< std::string >());

            for (auto k = 0U; k < ds[i]; k++)
              modals[i].insert(labels[k]);
          }
          */

          // while explicit learning does the right thing
          gum::learning::BNLearner< double > learner(
             GET_RESSOURCES_PATH("DBN_Tonda.csv"), learn1);
          learner.useScoreLog2Likelihood();
          learner.useAprioriSmoothing(1.0);
          learn2 = learner.learnParameters(dbn.dag());
        } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
      }
      gum::BayesNet< double > learn3;
      {
        // while explicit learning does the right thing
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("DBN_Tonda.csv"), dbn);
        learner.useScoreLog2Likelihood();
        learner.useAprioriSmoothing(1.0);
        learn3 = learner.learnParameters(dbn.dag());
      }

      TS_ASSERT_EQUALS(learn1.variable(learn1.idFromName("wl_0")).toString(),
                       "wl_0<0,1,2,3>");
      TS_ASSERT_EQUALS(learn2.variable(learn2.idFromName("wl_0")).toString(),
                       "wl_0<0,1,2,3>");
      TS_ASSERT_EQUALS(learn2.variable(learn3.idFromName("wl_0")).toString(),
                       "wl_0<0,1,2,3>");

      auto&              p1 = learn1.cpt(learn1.idFromName("c_0"));
      auto&              p2 = learn2.cpt(learn2.idFromName("c_0"));
      auto&              p3 = learn3.cpt(learn3.idFromName("c_0"));
      gum::Instantiation I1(p1), I2(p2), I3(p3);

      for (I1.setFirst(), I2.setFirst(), I3.setFirst(); !I1.end();
           I1.inc(), I2.inc(), I3.inc()) {
        TS_ASSERT_EQUALS(I1.toString(), I2.toString());   // same modalities orders
        TS_ASSERT_EQUALS(I1.toString(), I3.toString());   // same modalities orders
        TS_ASSERT_EQUALS(p1[I1], p2[I2]);                 // same probabilities
        TS_ASSERT_EQUALS(p1[I1], p3[I3]);                 // same probabilities
      }
    }


    void test_asia_with_missing_values() {
      int nb = 0;
      try {
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia3-faulty.csv"),
           std::vector< std::string >{"BEURK"});
        learner.useK2(std::vector< gum::NodeId >{1, 5, 2, 6, 0, 3, 4, 7});
        learner.learnBN();
      } catch (gum::MissingValueInDatabase&) { nb = 1; }

      TS_ASSERT(nb == 1);
    }

    void test_BugDoumenc() {
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

      std::vector< std::string > varContinuous{
         "A", "ADL"};   // les variables continues du RB


      std::vector< gum::NodeId > nodeList;   // Liste des noeuds du RB

      for (auto var : varBool)
        nodeList.push_back(templ.add(gum::LabelizedVariable(
           var,
           var,
           2)));   // Ajout des variables booléennes à la liste des noeuds

      for (auto var : varTer)
        nodeList.push_back(templ.add(gum::LabelizedVariable(
           var, var, 3)));   // Ajout des variables ternaires à la liste des noeuds

      gum::DiscretizedVariable< double > A("A", "A");
      for (int i = 60; i <= 105; i += 5) {
        A.addTick(double(i));
      }

      gum::NodeId a_id = templ.add(A);
      nodeList.push_back(a_id);   // Ajout de la variable Age allant de 60
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
      for (auto node : nodeList) {
        templ.addArc(iNRC, node);
      }

      gum::learning::BNLearner< double > learner(
         GET_RESSOURCES_PATH("bugDoumenc.csv"), templ);
      learner.useScoreLog2Likelihood();
      learner.useAprioriSmoothing();
      auto bn = learner.learnParameters(templ.dag());
    }

    void test_BugDoumencWithInt() {
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

      std::vector< std::string > varContinuous{
         "A", "ADL"};   // les variables continues du RB


      std::vector< gum::NodeId > nodeList;   // Liste des noeuds du RB

      for (auto var : varBool)
        nodeList.push_back(templ.add(gum::LabelizedVariable(
           var,
           var,
           2)));   // Ajout des variables booléennes à la liste des noeuds

      for (auto var : varTer)
        nodeList.push_back(templ.add(gum::LabelizedVariable(
           var, var, 3)));   // Ajout des variables ternaires à la liste des noeuds

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
      for (auto node : nodeList) {
        templ.addArc(iNRC, node);
      }


      gum::learning::BNLearner< double > learner(
         GET_RESSOURCES_PATH("bugDoumenc.csv"), templ);
      learner.useScoreLog2Likelihood();
      learner.useAprioriSmoothing();

      auto bn = learner.learnParameters(templ.dag());

      const gum::DiscreteVariable& var_discr = bn.variable("A");
      int                          good = 1;
      try {
        const gum::DiscretizedVariable< int >& xvar_discr =
           dynamic_cast< const gum::DiscretizedVariable< int >& >(var_discr);
        TS_ASSERT(xvar_discr.domainSize() == 9);
        TS_ASSERT(xvar_discr.label(0) == "[60;65[");
        TS_ASSERT(xvar_discr.label(1) == "[65;70[");
        TS_ASSERT(xvar_discr.label(8) == "[100;105]");
      } catch (std::bad_cast&) { good = 0; }
      TS_ASSERT(good == 1);
    }

    void test_setSliceOrderWithNames() {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia3.csv"));
      learner.setSliceOrder({{"smoking?", "lung_cancer?"},
                             {"bronchitis?", "visit_to_Asia?"},
                             {"tuberculosis?"}});


      gum::learning::BNLearner< double > learner2(
         GET_RESSOURCES_PATH("asia3.csv"));
      TS_ASSERT_THROWS(
         learner2.setSliceOrder({{"smoking?", "lung_cancer?"},
                                 {"bronchitis?", "visit_to_Asia?"},
                                 {"smoking?", "tuberculosis?", "lung_cancer?"}}),
         gum::DuplicateElement);

      gum::learning::BNLearner< double > learner3(
         GET_RESSOURCES_PATH("asia3.csv"));
      TS_ASSERT_THROWS(learner3.setSliceOrder({{"smoking?", "lung_cancer?"},
                                               {"bronchitis?", "visit_to_Asia?"},
                                               {"CRUCRU"}}),
                       gum::MissingVariableInDatabase);
    }

    void test_dirichlet() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D->E<-B");

      gum::learning::BNDatabaseGenerator< double > genere(bn);
      genere.setRandomVarOrder();
      genere.drawSamples(2000);
      genere.toCSV(GET_RESSOURCES_PATH("bnlearner_dirichlet.csv"));

      auto bn2 = gum::BayesNet< double >::fastPrototype("A->B->C->D->E");
      gum::learning::BNDatabaseGenerator< double > genere2(bn2);
      genere2.drawSamples(100);
      genere2.toCSV(GET_RESSOURCES_PATH("bnlearner_database.csv"));

      gum::learning::BNLearner< double > learner(
         GET_RESSOURCES_PATH("bnlearner_database.csv"), bn);
      learner.useAprioriDirichlet(GET_RESSOURCES_PATH("bnlearner_dirichlet.csv"),
                                  10);
      learner.useScoreAIC();

      try {
        gum::BayesNet< double > bn3 = learner.learnBN();
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    void test_dirichlet2() {
      // read the learning database
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("db_dirichlet_learning.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<>                translator_set;
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);


      // read the apriori database
      gum::learning::DBInitializerFromCSV<> dirichlet_initializer(
         GET_RESSOURCES_PATH("db_dirichlet_apriori.csv"));
      const auto&       dirichlet_var_names = initializer.variableNames();
      const std::size_t dirichlet_nb_vars = dirichlet_var_names.size();

      gum::learning::DBTranslatorSet<> dirichlet_translator_set;
      for (std::size_t i = 0; i < dirichlet_nb_vars; ++i) {
        dirichlet_translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> dirichlet_database(dirichlet_translator_set);
      dirichlet_database.setVariableNames(dirichlet_initializer.variableNames());
      dirichlet_initializer.fillDatabase(dirichlet_database);
      dirichlet_database.reorder();


      // create the score and the apriori
      gum::learning::DBRowGeneratorSet<>    dirichlet_genset;
      gum::learning::DBRowGeneratorParser<> dirichlet_parser(
         dirichlet_database.handler(), dirichlet_genset);
      gum::learning::AprioriDirichletFromDatabase<> apriori(dirichlet_database,
                                                            dirichlet_parser);

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);

      std::vector< double > weights{0, 1.0, 5.0, 10.0, 1000.0, 7000.0, 100000.0};

      gum::learning::BNLearner< double > learner(
         GET_RESSOURCES_PATH("db_dirichlet_learning.csv"));
      learner.useScoreBIC();

      for (const auto weight : weights) {
        apriori.setWeight(weight);
        gum::learning::ScoreBIC<> score(parser, apriori);

        // finalize the learning algorithm
        gum::learning::StructuralConstraintSetStatic<
           gum::learning::StructuralConstraintDAG >
           struct_constraint;

        gum::learning::ParamEstimatorML<> estimator(
           parser, apriori, score.internalApriori());

        gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
           op_set(struct_constraint);

        gum::learning::GraphChangesSelector4DiGraph< decltype(struct_constraint),
                                                     decltype(op_set) >
           selector(score, struct_constraint, op_set);

        gum::learning::GreedyHillClimbing search;

        gum::BayesNet< double > bn = search.learnBN(selector, estimator);
        // std::cout << dag << std::endl;


        learner.useAprioriDirichlet(
           GET_RESSOURCES_PATH("db_dirichlet_apriori.csv"), weight);

        gum::BayesNet< double > xbn = learner.learnBN();

        TS_ASSERT(xbn.moralGraph() == bn.moralGraph());
      }
    }

    void test_EM() {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("EM.csv"),
                                                 std::vector< std::string >{"?"});

      TS_ASSERT(learner.hasMissingValues());

      gum::DAG dag;
      for (std::size_t i = std::size_t(0); i < learner.database().nbVariables();
           ++i) {
        dag.addNodeWithId(gum::NodeId(i));
      }
      dag.addArc(gum::NodeId(1), gum::NodeId(0));
      dag.addArc(gum::NodeId(2), gum::NodeId(1));
      dag.addArc(gum::NodeId(3), gum::NodeId(2));

      TS_ASSERT_THROWS(learner.learnParameters(dag), gum::MissingValueInDatabase);

      learner.useEM(1e-3);
      learner.useAprioriSmoothing();

      TS_GUM_ASSERT_THROWS_NOTHING(learner.learnParameters(dag, false));
      TS_GUM_ASSERT_THROWS_NOTHING(learner.nbrIterations());
    }

    void test_chi2() {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia3.csv"));

      auto reschi2 = learner.chi2("smoking?", "lung_cancer?");
      TS_ASSERT_DELTA(reschi2.first, 36.2256, 1e-4);
      TS_ASSERT_DELTA(reschi2.second, 0, 1e-4);

      reschi2 = learner.chi2("smoking?", "visit_to_Asia?");
      TS_ASSERT_DELTA(reschi2.first, 1.1257, 1e-4);
      TS_ASSERT_DELTA(reschi2.second, 0.2886, 1e-4);

      reschi2 = learner.chi2("lung_cancer?", "tuberculosis?");
      TS_ASSERT_DELTA(reschi2.first, 0.6297, 1e-4);
      TS_ASSERT_DELTA(reschi2.second, 0.4274, 1e-4);

      reschi2 =
         learner.chi2("lung_cancer?", "tuberculosis?", {"tuberculos_or_cancer?"});
      TS_ASSERT_DELTA(reschi2.first, 58.0, 1e-4);
      TS_ASSERT_DELTA(reschi2.second, 0.0, 1e-4);

      // see IndepTestChi2TestSuite::test_statistics
      gum::learning::BNLearner< double > learner2(GET_RESSOURCES_PATH("chi2.csv"));

      auto stat = learner2.chi2("A", "C");
      TS_ASSERT_DELTA(stat.first, 0.0007, 1e-3);
      TS_ASSERT_DELTA(stat.second, 0.978, 1e-3);

      stat = learner2.chi2("A", "B");
      TS_ASSERT_DELTA(stat.first, 21.4348, 1e-3);
      TS_ASSERT_DELTA(stat.second, 3.6e-6, 1e-5);

      stat = learner2.chi2("B", "A");
      TS_ASSERT_DELTA(stat.first, 21.4348, 1e-3);
      TS_ASSERT_DELTA(stat.second, 3.6e-6, 1e-5);

      stat = learner2.chi2("B", "D");
      TS_ASSERT_DELTA(stat.first, 0.903, 1e-3);
      TS_ASSERT_DELTA(stat.second, 0.341, 1e-3);

      stat = learner2.chi2("A", "C", {"B"});
      TS_ASSERT_DELTA(stat.first, 15.2205, 1e-3);
      TS_ASSERT_DELTA(stat.second, 0.0005, 1e-4);
    }

    void test_G2() {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia3.csv"));

      auto resg2 = learner.G2("smoking?", "lung_cancer?");
      TS_ASSERT_DELTA(resg2.first, 43.0321, 1e-4);
      TS_ASSERT_DELTA(resg2.second, 0, 1e-4);

      resg2 = learner.G2("smoking?", "visit_to_Asia?");
      TS_ASSERT_DELTA(resg2.first, 1.1418, 1e-4);
      TS_ASSERT_DELTA(resg2.second, 0.2852, 1e-4);

      resg2 = learner.G2("lung_cancer?", "tuberculosis?");
      TS_ASSERT_DELTA(resg2.first, 1.2201, 1e-4);
      TS_ASSERT_DELTA(resg2.second, 0.2693, 1e-4);

      resg2 =
         learner.G2("lung_cancer?", "tuberculosis?", {"tuberculos_or_cancer?"});
      TS_ASSERT_DELTA(resg2.first, 59.1386, 1e-4);
      TS_ASSERT_DELTA(resg2.second, 0.0, 1e-4);

      // see IndepTestChi2TestSuite::test_statistics
      gum::learning::BNLearner< double > learner2(GET_RESSOURCES_PATH("chi2.csv"));

      auto stat = learner2.G2("A", "C");
      TS_ASSERT_DELTA(stat.first, 0.0007, 1e-3);
      TS_ASSERT_DELTA(stat.second, 0.978, 1e-4);

      stat = learner2.G2("A", "B");
      TS_ASSERT_DELTA(stat.first, 21.5846, 1e-3);
      TS_ASSERT_DELTA(stat.second, 3.6e-6, 1e-4);

      stat = learner2.G2("B", "A");
      TS_ASSERT_DELTA(stat.first, 21.5846, 1e-3);
      TS_ASSERT_DELTA(stat.second, 3.6e-6, 1e-4);

      stat = learner2.G2("B", "D");
      TS_ASSERT_DELTA(stat.first, 0.903, 1e-3);
      TS_ASSERT_DELTA(stat.second, 0.342, 1e-4);

      stat = learner2.G2("A", "C", {"B"});
      TS_ASSERT_DELTA(stat.first, 16.3470, 1e-3);
      TS_ASSERT_DELTA(stat.second, 0.0002, 1e-4);
    }

    void test_cmpG2Chi2() {
      gum::learning::BNLearner< double > learner(
         GET_RESSOURCES_PATH("testXYbase.csv"));
      auto statChi2 = learner.chi2("X", "Y");
      TS_ASSERT_DELTA(statChi2.first, 15.3389, 1e-3);
      TS_ASSERT_DELTA(statChi2.second, 0.01777843046460533, 1e-6);
      auto statG2 = learner.G2("X", "Y");
      TS_ASSERT_DELTA(statG2.first, 16.6066, 1e-3);
      TS_ASSERT_DELTA(statG2.second, 0.0108433, 1e-6);
    }

    void test_loglikelihood() {
      gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("chi2.csv"));
      TS_ASSERT_EQUALS(learner.nbRows(), (gum::Size)500);
      TS_ASSERT_EQUALS(learner.nbCols(), (gum::Size)4);

      double siz = -1.0 * learner.database().size();
      learner.useNoApriori();

      auto stat = learner.logLikelihood({"A"}) / siz;   // LL=-N.H
      TS_ASSERT_DELTA(stat, 0.99943499, 1e-5);
      stat = learner.logLikelihood({"B"}) / siz;   // LL=-N.H
      TS_ASSERT_DELTA(stat, 0.9986032, 1e-5);
      stat = learner.logLikelihood({std::string("A"), "B"}) / siz;   // LL=-N.H
      TS_ASSERT_DELTA(stat, 1.9668973, 1e-5);
      stat = learner.logLikelihood({std::string("A")}, {"B"}) / siz;   // LL=-N.H
      TS_ASSERT_DELTA(stat, 1.9668973 - 0.9986032, 1e-5);

      stat = learner.logLikelihood({"C"}) / siz;   // LL=-N.H
      TS_ASSERT_DELTA(stat, 0.99860302, 1e-5);
      stat = learner.logLikelihood({"D"}) / siz;   // LL=-N.H
      TS_ASSERT_DELTA(stat, 0.40217919, 1e-5);
      stat = learner.logLikelihood({std::string("C"), "D"}) / siz;   // LL=-N.H
      TS_ASSERT_DELTA(stat, 1.40077995, 1e-5);
      stat = learner.logLikelihood({std::string("C")}, {"D"}) / siz;   // LL=-N.H
      TS_ASSERT_DELTA(stat, 1.40077995 - 0.40217919, 1e-5);
    }

    void test_errorFromPyagrum() {
      try {
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("sample_asia.csv"));
        learner.use3off2();
        learner.useNML();
        auto ge3off2 = learner.learnMixedStructure();
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    void test_PossibleEdges() {
      //[smoking? , lung_cancer? , bronchitis? , visit_to_Asia? , tuberculosis? ,
      // tuberculos_or_cancer? , dyspnoea? , positive_XraY?]
      {
        // possible edges are not relevant
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia3.csv"));
        learner.addPossibleEdge("visit_to_Asia?", "lung_cancer?");
        learner.addPossibleEdge("visit_to_Asia?", "smoking?");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), gum::Size(0));
      }

      {
        // possible edges are relevant
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia3.csv"));
        learner.addPossibleEdge("smoking?", "lung_cancer?");
        learner.addPossibleEdge("bronchitis?", "smoking?");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), gum::Size(2));
        TS_ASSERT(bn.parents("lung_cancer?").contains(bn.idFromName("smoking?")));
        TS_ASSERT(bn.parents("bronchitis?").contains(bn.idFromName("smoking?")));
      }

      {
        // possible edges are relevant
        // mixed with a forbidden arcs
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia3.csv"));
        learner.addPossibleEdge("smoking?", "lung_cancer?");
        learner.addPossibleEdge("bronchitis?", "smoking?");
        learner.addForbiddenArc("smoking?", "bronchitis?");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), gum::Size(2));
        TS_ASSERT(bn.parents("lung_cancer?").contains(bn.idFromName("smoking?")));
        TS_ASSERT(bn.parents("smoking?").contains(bn.idFromName("bronchitis?")));
      }

      {
        // possible edges are relevant
        // mixed with a mandatory arcs
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia3.csv"));
        learner.addPossibleEdge("smoking?", "lung_cancer?");
        learner.addPossibleEdge("bronchitis?", "smoking?");
        learner.addMandatoryArc("visit_to_Asia?", "bronchitis?");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), gum::Size(3));
        TS_ASSERT(bn.parents("lung_cancer?").contains(bn.idFromName("smoking?")));
        TS_ASSERT(bn.parents("smoking?").contains(bn.idFromName("bronchitis?")));
        TS_ASSERT(
           bn.parents("bronchitis?").contains(bn.idFromName("visit_to_Asia?")));
      }
    }

    void test_PossibleEdgesTabu() {
      //[smoking? , lung_cancer? , bronchitis? , visit_to_Asia? , tuberculosis? ,
      // tuberculos_or_cancer? , dyspnoea? , positive_XraY?]
      {
        // possible edges are not relevant
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia3.csv"));
        learner.useLocalSearchWithTabuList();
        learner.addPossibleEdge("visit_to_Asia?", "lung_cancer?");
        learner.addPossibleEdge("visit_to_Asia?", "smoking?");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), gum::Size(0));
      }

      {
        // possible edges are relevant
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia3.csv"));
        learner.useLocalSearchWithTabuList();
        learner.addPossibleEdge("smoking?", "lung_cancer?");
        learner.addPossibleEdge("bronchitis?", "smoking?");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), gum::Size(2));
        TS_ASSERT(bn.parents("lung_cancer?").contains(bn.idFromName("smoking?")));
        TS_ASSERT(bn.parents("bronchitis?").contains(bn.idFromName("smoking?")));
      }

      {
        // possible edges are relevant
        // mixed with a forbidden arcs
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia3.csv"));
        learner.useLocalSearchWithTabuList();
        learner.addPossibleEdge("smoking?", "lung_cancer?");
        learner.addPossibleEdge("bronchitis?", "smoking?");
        learner.addForbiddenArc("smoking?", "bronchitis?");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), gum::Size(2));
        TS_ASSERT(bn.parents("lung_cancer?").contains(bn.idFromName("smoking?")));
        TS_ASSERT(bn.parents("smoking?").contains(bn.idFromName("bronchitis?")));
      }

      {
        // possible edges are relevant
        // mixed with a mandatory arcs
        gum::learning::BNLearner< double > learner(
           GET_RESSOURCES_PATH("asia3.csv"));
        learner.useLocalSearchWithTabuList();
        learner.addPossibleEdge("smoking?", "lung_cancer?");
        learner.addPossibleEdge("bronchitis?", "smoking?");
        learner.addMandatoryArc("visit_to_Asia?", "bronchitis?");

        gum::BayesNet< double > bn = learner.learnBN();
        TS_ASSERT_EQUALS(bn.sizeArcs(), gum::Size(3));
        TS_ASSERT(bn.parents("lung_cancer?").contains(bn.idFromName("smoking?")));
        TS_ASSERT(bn.parents("bronchitis?").contains(bn.idFromName("smoking?")));
        TS_ASSERT(
           bn.parents("bronchitis?").contains(bn.idFromName("visit_to_Asia?")));
      }
    }

    void testNonRegressionZeroCount() {
      try {
        //////////////////////////
        // without specific score
        auto templ12 =
           gum::BayesNet< double >::fastPrototype("smoking?->lung_cancer?");
        gum::learning::BNLearner< double > learner(GET_RESSOURCES_PATH("asia.csv"),
                                                   templ12);
        auto bn = learner.learnParameters(templ12.dag());

        gum::learning::BNLearner< double > learner2(
           GET_RESSOURCES_PATH("asia.csv"), templ12);
        auto bn2 = learner2.learnParameters(templ12.dag());
        TS_ASSERT_EQUALS(bn.cpt("lung_cancer?").toString(),
                         bn2.cpt("lung_cancer?").toString());

        //////////////////////////
        // with score AIC
        auto templ34 =
           gum::BayesNet< double >::fastPrototype("smoking?[3]->lung_cancer?[3]");
        gum::learning::BNLearner< double > learner3(
           GET_RESSOURCES_PATH("asia.csv"), templ34);
        learner3.useScoreAIC();

#ifdef GUM_PARAMESTIMATOR_ERROR_WHEN_NIJ_IS_NULL
        TS_ASSERT_THROWS(learner3.learnParameters(templ34.dag()),
                         gum::DatabaseError)
#else   // GUM_PARAMESTIMATOR_ERROR_WHEN_NIJ_IS_NULL
        auto bn3 = learner3.learnParameters(templ34.dag());
        {
          const gum::Potential< double >& p = bn.cpt("lung_cancer?");
          const gum::Potential< double >& q = bn3.cpt("lung_cancer?");

          auto I = gum::Instantiation(p);
          auto J = gum::Instantiation(q);

          TS_ASSERT_DELTA(p[I], q[J], 1e-6);
          ++I;
          ++J;
          TS_ASSERT_DELTA(p[I], q[J], 1e-6);
          ++J;
          TS_ASSERT_DELTA(0.0, q[J], 1e-6);
          ++I;
          ++J;
          TS_ASSERT_DELTA(p[I], q[J], 1e-6);
          ++I;
          ++J;
          TS_ASSERT_DELTA(p[I], q[J], 1e-6);
          ++J;
          TS_ASSERT_DELTA(0.0, q[J], 1e-6);
          ++J;
          TS_ASSERT_DELTA(1.0 / 3.0, q[J], 1e-6);
          ++J;
          TS_ASSERT_DELTA(1.0 / 3.0, q[J], 1e-6);
          ++J;
          TS_ASSERT_DELTA(1.0 / 3.0, q[J], 1e-6);
        }
#endif

        gum::learning::BNLearner< double > learner4(
           GET_RESSOURCES_PATH("asia.csv"), templ34);
        learner4.useScoreAIC();

#ifdef GUM_PARAMESTIMATOR_ERROR_WHEN_NIJ_IS_NULL
        TS_ASSERT_THROWS(learner4.learnParameters(templ34.dag()),
                         gum::DatabaseError)
#else   // GUM_PARAMESTIMATOR_ERROR_WHEN_NIJ_IS_NULL
        auto bn4 = learner4.learnParameters(templ34.dag());
        {
          const gum::Potential< double >& p = bn.cpt("lung_cancer?");
          const gum::Potential< double >& q = bn4.cpt("lung_cancer?");

          auto I = gum::Instantiation(p);
          auto J = gum::Instantiation(q);

          TS_ASSERT_DELTA(p[I], q[J], 1e-6);
          ++I;
          ++J;
          TS_ASSERT_DELTA(p[I], q[J], 1e-6);
          ++J;
          TS_ASSERT_DELTA(0.0, q[J], 1e-6);
          ++I;
          ++J;
          TS_ASSERT_DELTA(p[I], q[J], 1e-6);
          ++I;
          ++J;
          TS_ASSERT_DELTA(p[I], q[J], 1e-6);
          ++J;
          TS_ASSERT_DELTA(0.0, q[J], 1e-6);
          ++J;
          TS_ASSERT_DELTA(1.0 / 3.0, q[J], 1e-6);
          ++J;
          TS_ASSERT_DELTA(1.0 / 3.0, q[J], 1e-6);
          ++J;
          TS_ASSERT_DELTA(1.0 / 3.0, q[J], 1e-6);
        }
#endif
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }
  };
} /* namespace gum_tests */
