
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


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/aprioris/aprioriNoApriori.h>
#include <agrum/learning/aprioris/aprioriSmoothing.h>
#include <agrum/learning/paramUtils/paramEstimatorML.h>
#include <agrum/learning/paramUtils/DAG2BNLearner.h>
#include <agrum/learning/database/DBRowGenerator4CompleteRows.h>
#include <agrum/learning/database/DBRowGeneratorEM.h>

namespace gum_tests {

  class DAG2BNLearnerTestSuite : public CxxTest::TestSuite {
    private:
    std::vector< double > __normalize(const std::vector< double >& vin) {
      double sum = 0;
      for (const auto& val : vin)
        sum += val;
      std::vector< double > vout(vin);
      for (auto& val : vout)
        val /= sum;
      return vout;
    }

    std::vector< double > __xnormalize(const std::vector< double >& vin) {
      std::vector< double > vout(vin);
      for (std::size_t i = 0; i < vin.size(); i += 3) {
        double sum = 0;
        for (std::size_t j = std::size_t(0); j < 3; ++j)
          sum += vin[i + j];
        for (std::size_t j = std::size_t(0); j < 3; ++j)
          vout[i + j] /= sum;
      }
      return vout;
    }

    std::vector< double > __getProba(const gum::BayesNet< double >& bn,
                                     const gum::NodeId              id) {
      const gum::Potential< double >& pot = bn.cpt(id);
      std::vector< double >           vect;
      for (gum::Instantiation inst(pot); !inst.end(); ++inst) {
        vect.push_back(pot.get(inst));
      }
      return vect;
    }

    public:
    void test1() {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet<> trans_set;
      {
        const std::vector< std::string >                miss;
        gum::learning::DBTranslator4LabelizedVariable<> translator(var, miss);
        std::vector< std::string > names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable<> database(trans_set);
      std::vector< std::string >     row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >     row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >     row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >     row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >     row4{"0", "0", "0", "1", "1", "1"};
      for (int i = 0; i < 1000; ++i)
        database.insertRow(row0);
      for (int i = 0; i < 50; ++i)
        database.insertRow(row1);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row2);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row3);
      for (int i = 0; i < 200; ++i)
        database.insertRow(row4);

      // create the parser
      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
      gum::learning::AprioriSmoothing<>     extern_apriori(database);
      gum::learning::AprioriNoApriori<>     intern_apriori(database);

      gum::learning::ParamEstimatorML<> param_estimator(
         parser, extern_apriori, intern_apriori);

      gum::learning::DAG2BNLearner<> learner;

      gum::DAG dag;
      for (std::size_t i = std::size_t(0); i < database.nbVariables(); ++i) {
        dag.addNodeWithId(gum::NodeId(i));
      }
      dag.addArc(0, 1);
      dag.addArc(2, 0);

      auto bn1 = learner.createBN(param_estimator, dag);

      auto                  v2 = __getProba(bn1, 2);
      std::vector< double > xv2 = __normalize({1401, 1, 1});
      TS_ASSERT(v2 == xv2);

      auto                  v02 = __getProba(bn1, 0);
      std::vector< double > xv02 = __xnormalize({1201, 126, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT(v02 == xv02);
    }


    void testEM() {
      gum::LabelizedVariable var("x", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      gum::learning::DBTranslatorSet<> trans_set;
      {
        const std::vector< std::string >                miss{"N/A", "?"};
        gum::learning::DBTranslator4LabelizedVariable<> translator(var, miss);
        std::vector< std::string >                      names{"A", "B", "C", "D"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      gum::learning::DatabaseTable<> database(trans_set);
      std::vector< std::string >     row1{"0", "1", "1", "0"};
      std::vector< std::string >     row2{"0", "?", "0", "1"};
      std::vector< std::string >     row3{"1", "?", "?", "0"};
      std::vector< std::string >     row4{"?", "?", "1", "0"};
      std::vector< std::string >     row5{"?", "0", "?", "?"};
      for (int i = 0; i < 100; ++i) {
        database.insertRow(row1);
        database.insertRow(row2);
        database.insertRow(row3);
        database.insertRow(row4);
        database.insertRow(row5);
      }

      const std::vector< gum::learning::DBTranslatedValueType > col_types{
         gum::learning::DBTranslatedValueType::DISCRETE,
         gum::learning::DBTranslatedValueType::DISCRETE,
         gum::learning::DBTranslatedValueType::DISCRETE,
         gum::learning::DBTranslatedValueType::DISCRETE};

      auto bn = gum::BayesNet< double >::fastPrototype("A;B;C;D");
      bn.cpt("A").fillWith({0.3, 0.7});
      bn.cpt("B").fillWith({0.3, 0.7});
      bn.cpt("C").fillWith({0.3, 0.7});
      bn.cpt("D").fillWith({0.3, 0.7});

      // bugfix for parallel exceution of VariableElimination
      {
        const gum::DAG& dag = bn.dag();
        for (const auto node : dag) {
          dag.parents(node);
          dag.children(node);
        }
      }

      // create the parser
      gum::learning::DBRowGenerator4CompleteRows<> generator_id(col_types);
      gum::learning::DBRowGeneratorSet<>           genset_id;
      genset_id.insertGenerator(generator_id);
      gum::learning::DBRowGeneratorParser<> parser_id(database.handler(),
                                                      genset_id);

      gum::learning::AprioriSmoothing<> extern_apriori(database);
      gum::learning::AprioriNoApriori<> intern_apriori(database);
      gum::learning::ParamEstimatorML<> param_estimator_id(
         parser_id, extern_apriori, intern_apriori);

      gum::learning::DBRowGeneratorEM<> generator_EM(col_types, bn);
      gum::learning::DBRowGenerator<>&  gen_EM = generator_EM;   // fix for g++-4.8
      gum::learning::DBRowGeneratorSet<> genset_EM;
      genset_EM.insertGenerator(gen_EM);
      gum::learning::DBRowGeneratorParser<> parser_EM(database.handler(),
                                                      genset_EM);
      gum::learning::ParamEstimatorML<>     param_estimator_EM(
         parser_EM, extern_apriori, intern_apriori);

      gum::learning::DAG2BNLearner<> learner;

      gum::DAG dag;
      for (std::size_t i = std::size_t(0); i < database.nbVariables(); ++i) {
        dag.addNodeWithId(gum::NodeId(i));
      }
      dag.addArc(gum::NodeId(1), gum::NodeId(0));
      dag.addArc(gum::NodeId(2), gum::NodeId(1));
      dag.addArc(gum::NodeId(3), gum::NodeId(2));

      learner.setEpsilon(1e-3);
      bool ok;
      for (int i = 0; i < 10; i++) {
        ok = true;
        auto bn1 = learner.createBN(param_estimator_id, param_estimator_EM, dag);
        auto margB = (bn1.cpt("D") * bn1.cpt("C") * bn1.cpt("B"))
                        .margSumIn(gum::Set< const gum::DiscreteVariable* >(
                           {&bn1.variableFromName("B")}));
        if ((bn1.cpt("D").max() < 0.8) && (bn1.cpt("D").max() > 0.6)
            && (margB.max() > 0.5) && (margB.max() < 0.6))
          break;
        ok = false;
      }
      TS_ASSERT(ok);
    }
  };

}   // namespace gum_tests
