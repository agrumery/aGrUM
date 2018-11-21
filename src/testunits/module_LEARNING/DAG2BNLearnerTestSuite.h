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

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/aprioris/aprioriNoApriori.h>
#include <agrum/learning/aprioris/aprioriSmoothing.h>
#include <agrum/learning/paramUtils/paramEstimatorML.h>
#include <agrum/learning/paramUtils/DAG2BNLearner.h>

namespace gum_tests {

  class DAG2BNLearnerTestSuite : public CxxTest::TestSuite {
  private:
    std::vector<double> __normalize (const std::vector<double>& vin) {
      double sum = 0;
      for ( const auto& val : vin ) sum += val;
      std::vector<double> vout(vin);
      for ( auto& val : vout ) val /= sum;
      return vout;
    }

    std::vector<double> __xnormalize (const std::vector<double>& vin) {
      std::vector<double> vout(vin);
      for ( std::size_t i = 0; i < vin.size(); i += 3) {
        double sum = 0;
        for ( std::size_t j = std::size_t(0); j < 3; ++j ) sum += vin[i+j];
        for ( std::size_t j = std::size_t(0); j < 3; ++j ) vout[i+j] /= sum;
      }
      return vout;
    }

    std::vector<double> __getProba ( const gum::BayesNet<double>& bn,
                                     const gum::NodeId id) {
      const gum::Potential< double >& pot = bn.cpt(id);
      std::vector< double > vect;
      for ( gum::Instantiation inst (pot); ! inst.end(); ++inst ) {
        vect.push_back ( pot.get(inst));
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
      gum::learning::AprioriSmoothing<>    extern_apriori(database);
      gum::learning::AprioriNoApriori<>    intern_apriori(database);

      gum::learning::ParamEstimatorML<>
        param_estimator(parser,extern_apriori,intern_apriori);

      gum::learning::DAG2BNLearner<> learner;

      gum::DAG dag;
      for (std::size_t i = std::size_t(0); i < database.nbVariables(); ++i) {
        dag.addNodeWithId ( gum::NodeId(i) );
      }
      dag.addArc(0,1);
      dag.addArc(2,0);
      
      auto bn1 = learner.createBN(param_estimator,dag);

      auto v2 = __getProba ( bn1, 2);
      std::vector<double> xv2 = __normalize({1401, 1, 1});
      TS_ASSERT(v2 == xv2);

      auto v02 = __getProba ( bn1, 0);
      std::vector<double> xv02 = __xnormalize ({1201,126,76,1,1,1,1,1,1});
      TS_ASSERT(v02 == xv02);
      
    }

  };

} // namespace

