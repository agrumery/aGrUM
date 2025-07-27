/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>

#include <agrum/base/database/DBRowGenerator4CompleteRows.h>
#include <agrum/base/database/DBRowGeneratorEM.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/BN/learning/paramUtils/DAG2BNLearner.h>
#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>
#include <agrum/BN/learning/priors/noPrior.h>
#include <agrum/BN/learning/priors/smoothingPrior.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  class [[maybe_unused]] DAG2BNLearnerTestSuite: public CxxTest::TestSuite {
    private:
    std::vector< double > _normalize_(const std::vector< double >& vin) {
      double sum = 0;
      for (const auto& val: vin)
        sum += val;
      std::vector< double > vout(vin);
      for (auto& val: vout)
        val /= sum;
      return vout;
    }

    std::vector< double > _xnormalize_(const std::vector< double >& vin) {
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

    std::vector< double > _getProba_(const gum::BayesNet< double >& bn, const gum::NodeId id) {
      const gum::Tensor< double >& pot = bn.cpt(id);
      std::vector< double >        vect;
      for (gum::Instantiation inst(pot); !inst.end(); ++inst) {
        vect.push_back(pot.get(inst));
      }
      return vect;
    }

    public:
    GUM_ACTIVE_TEST(1) {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet trans_set;
      {
        const std::vector< std::string >              miss;
        gum::learning::DBTranslator4LabelizedVariable translator(var, miss);
        std::vector< std::string >                    names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable database(trans_set);
      std::vector< std::string >   row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >   row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >   row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >   row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >   row4{"0", "0", "0", "1", "1", "1"};
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
      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::SmoothingPrior       extern_prior(database);
      gum::learning::NoPrior              intern_prior(database);

      gum::learning::ParamEstimatorML param_estimator(parser, extern_prior, intern_prior);

      gum::learning::DAG2BNLearner learner;

      gum::DAG dag;
      for (std::size_t i = std::size_t(0); i < database.nbVariables(); ++i) {
        dag.addNodeWithId(gum::NodeId(i));
      }
      dag.addArc(0, 1);
      dag.addArc(2, 0);

      auto bn1 = learner.createBN(param_estimator, dag);

      auto                  v2  = _getProba_(bn1, 2);
      std::vector< double > xv2 = _normalize_({1401, 1, 1});
      TS_ASSERT_EQUALS(v2, xv2)

      auto                  v02  = _getProba_(bn1, 0);
      std::vector< double > xv02 = _xnormalize_({1201, 126, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT_EQUALS(v02, xv02)
    }

    GUM_ACTIVE_TEST(EM) {
      gum::LabelizedVariable var("x", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      gum::learning::DBTranslatorSet trans_set;
      {
        const std::vector< std::string >              miss{"N/A", "?"};
        gum::learning::DBTranslator4LabelizedVariable translator(var, miss);
        std::vector< std::string >                    names{"A", "B", "C", "D"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      gum::learning::DatabaseTable database(trans_set);
      std::vector< std::string >   row1{"0", "1", "1", "0"};
      std::vector< std::string >   row2{"0", "?", "0", "1"};
      std::vector< std::string >   row3{"1", "?", "?", "0"};
      std::vector< std::string >   row4{"?", "?", "1", "0"};
      std::vector< std::string >   row5{"?", "0", "?", "?"};
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

      // bugfix for parallel execution of VariableElimination
      {
        const gum::DAG& dag = bn.dag();
        for (const auto node: dag) {
          dag.parents(node);
          dag.children(node);
        }
      }

      // create the parser
      gum::learning::DBRowGenerator4CompleteRows generator_id(col_types);
      gum::learning::DBRowGeneratorSet           genset_id;
      genset_id.insertGenerator(generator_id);
      gum::learning::DBRowGeneratorParser parser_id(database.handler(), genset_id);

      gum::learning::SmoothingPrior   extern_prior(database);
      gum::learning::NoPrior          intern_prior(database);
      gum::learning::ParamEstimatorML param_estimator_id(parser_id, extern_prior, intern_prior);

      gum::learning::DBRowGeneratorEM  generator_EM(col_types, bn);
      gum::learning::DBRowGenerator&   gen_EM = generator_EM;   // fix for g++-4.8
      gum::learning::DBRowGeneratorSet genset_EM;
      genset_EM.insertGenerator(gen_EM);
      gum::learning::DBRowGeneratorParser parser_EM(database.handler(), genset_EM);
      gum::learning::ParamEstimatorML     param_estimator_EM(parser_EM, extern_prior, intern_prior);

      gum::learning::DAG2BNLearner learner;

      gum::DAG dag;
      for (std::size_t i = std::size_t(0); i < database.nbVariables(); ++i) {
        dag.addNodeWithId(gum::NodeId(i));
      }
      dag.addArc(gum::NodeId(1), gum::NodeId(0));
      dag.addArc(gum::NodeId(2), gum::NodeId(1));
      dag.addArc(gum::NodeId(3), gum::NodeId(2));

      learner.setMinEpsilonRate(1e-3);
      learner.setNoise(0.15);
      bool ok;
      for (int i = 0; i < 10; i++) {
        ok         = true;
        auto bn1   = learner.createBNwithEM(param_estimator_id, param_estimator_EM, dag);
        auto margB = (bn1.cpt("D") * bn1.cpt("C") * bn1.cpt("B"))
                         .sumIn(gum::VariableSet({&bn1.variableFromName("B")}));
        if ((bn1.cpt("D").max() < 0.8) && (bn1.cpt("D").max() > 0.6) && (margB.max() > 0.5)
            && (margB.max() < 0.6))
          break;
        ok = false;
      }
      TS_ASSERT(ok)

      gum::BayesNet< double > xbn  = bn;
      auto&                   cpt0 = xbn.cpt(0);
      cpt0.fillWith(std::vector(cpt0.domainSize(), 0.0));
      auto& cpt2 = xbn.cpt(2);
      cpt2.fillWith(std::vector(cpt2.domainSize(), 0.0));
      for (int i = 0; i < 5; i++) {
        ok         = true;
        auto bn2   = learner.createBNwithEM(param_estimator_id, param_estimator_EM, xbn);
        auto margB = (bn2.cpt("D") * bn2.cpt("C") * bn2.cpt("B"))
                         .sumIn(gum::VariableSet({&bn2.variableFromName("B")}));
        if ((bn2.cpt("D").max() < 0.8) && (bn2.cpt("D").max() > 0.6) && (margB.max() > 0.5)
            && (margB.max() < 0.6))
          break;
        ok = false;
      }
      TS_ASSERT(ok)
    }
  };

}   // namespace gum_tests
