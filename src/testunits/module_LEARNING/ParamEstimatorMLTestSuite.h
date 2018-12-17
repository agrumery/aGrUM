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

#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/learning/database/DBRowGeneratorEM.h>
#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/aprioris/aprioriNoApriori.h>
#include <agrum/learning/aprioris/aprioriSmoothing.h>
#include <agrum/learning/paramUtils/paramEstimatorML.h>

namespace gum_tests {

  class ParamEstimatorMLTestSuite : public CxxTest::TestSuite {
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
  
    gum::Potential<double>
    __infer( const gum::BayesNet<double>& bn,
             const std::vector<std::size_t>& targets,
             const gum::learning::DBRow< gum::learning::DBTranslatedValue >& row) {
      gum::LazyPropagation<double> ve(&bn);
      
      gum::NodeSet target_set;
      for (auto target : targets) target_set.insert(gum::NodeId(target));
      ve.addJointTarget(target_set);

      const auto xrow = row.row();
      const auto row_size = xrow.size();
      for (std::size_t col = std::size_t(0); col < row_size; ++col) {
        if ( xrow[col].discr_val != std::numeric_limits<std::size_t>::max() ) {
          ve.addEvidence(gum::NodeId(col), xrow[col].discr_val);
        }
      }

      gum::Potential<double> prob = ve.jointPosterior(target_set);
      return prob;
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

      std::vector< double > v0 = param_estimator.parameters(gum::NodeId(0));
      std::vector< double > xv0 = __normalize({1201, 126, 76});
      TS_ASSERT(v0 == xv0);

      std::vector< double > v1 = param_estimator.parameters(gum::NodeId(1));
      std::vector< double > xv1 = __normalize({276, 1076, 51});
      TS_ASSERT(v1 == xv1);

      std::vector< double > v2 = param_estimator.parameters(gum::NodeId(2));
      std::vector< double > xv2 = __normalize({1401, 1, 1});
      TS_ASSERT(v2 == xv2);

      std::vector< double > v02 =
        param_estimator.parameters(gum::NodeId(0), {gum::NodeId(2)});
      std::vector< double > xv02 = __xnormalize({1201, 126, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT(v02 == xv02);

      std::vector< double > v01 =
        param_estimator.parameters(gum::NodeId(0), {gum::NodeId(1)});
      std::vector< double > xv01 =
        __xnormalize({201, 76, 1, 1001, 1, 76, 1, 51, 1});
      TS_ASSERT(v01 == xv01);

      std::vector< double > v21 =
        param_estimator.parameters(gum::NodeId(2), {gum::NodeId(1)});
      std::vector< double > xv21 = __xnormalize({276, 1, 1, 1076, 1, 1, 51, 1, 1});
      TS_ASSERT(v21 == xv21);


      gum::learning::ParamEstimatorML<> param_estimator2(param_estimator);
      std::vector< double > w0 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT(w0 == xv0);

      std::vector< double > w1 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT(w1 == xv1);

      std::vector< double > w2 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT(w2 == xv2);

      std::vector< double > w02 =
        param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(w02 == xv02);

      std::vector< double > w01 =
        param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT(w01 == xv01);

      std::vector< double > w21 =
        param_estimator2.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT(w21 == xv21);


      gum::learning::ParamEstimatorML<> param_estimator3(
        std::move(param_estimator2));
      std::vector< double > x0 = param_estimator3.parameters(gum::NodeId(0));
      TS_ASSERT(x0 == xv0);

      std::vector< double > x1 = param_estimator3.parameters(gum::NodeId(1));
      TS_ASSERT(x1 == xv1);

      std::vector< double > x2 = param_estimator3.parameters(gum::NodeId(2));
      TS_ASSERT(x2 == xv2);

      std::vector< double > x02 =
        param_estimator3.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(x02 == xv02);

      std::vector< double > x01 =
        param_estimator3.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT(x01 == xv01);

      std::vector< double > x21 =
        param_estimator3.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT(x21 == xv21);


      gum::learning::ParamEstimatorML<>* param_estimator4 =
        param_estimator.clone();
      std::vector< double > y0 = param_estimator4->parameters(gum::NodeId(0));
      TS_ASSERT(y0 == xv0);

      std::vector< double > y1 = param_estimator4->parameters(gum::NodeId(1));
      TS_ASSERT(y1 == xv1);

      std::vector< double > y2 = param_estimator4->parameters(gum::NodeId(2));
      TS_ASSERT(y2 == xv2);

      std::vector< double > y02 =
        param_estimator4->parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(y02 == xv02);

      std::vector< double > y01 =
        param_estimator4->parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT(y01 == xv01);

      std::vector< double > y21 =
        param_estimator4->parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT(y21 == xv21);

      delete param_estimator4;
    }


    void test2() {
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

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000},
                                                                  {1050, 1400}};

      gum::learning::ParamEstimatorML<> param_estimator(
        parser, extern_apriori, intern_apriori, ranges);

      std::vector< double > r0 = param_estimator.parameters(gum::NodeId(0));
      std::vector< double > xr0 = __normalize({401, 76, 76});
      TS_ASSERT(r0 == xr0);

      std::vector< double > r1 = param_estimator.parameters(gum::NodeId(1));
      std::vector< double > xr1 = __normalize({276, 276, 1});
      TS_ASSERT(r1 == xr1);

      std::vector< double > r2 = param_estimator.parameters(gum::NodeId(2));
      std::vector< double > xr2 = __normalize({551, 1, 1});
      TS_ASSERT(r2 == xr2);

      std::vector< double > r02 =
        param_estimator.parameters(gum::NodeId(0), {gum::NodeId(2)});
      std::vector< double > xr02 = __xnormalize({401, 76, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT(r02 == xr02);

      std::vector< double > r01 =
        param_estimator.parameters(gum::NodeId(0), {gum::NodeId(1)});
      std::vector< double > xr01 = __xnormalize({201, 76, 1, 201, 1, 76, 1, 1, 1});
      TS_ASSERT(r01 == xr01);

      std::vector< double > r21 =
        param_estimator.parameters(gum::NodeId(2), {gum::NodeId(1)});
      std::vector< double > xr21 = __xnormalize({276, 1, 1, 276, 1, 1, 1, 1, 1});
      TS_ASSERT(r21 == xr21);


      gum::learning::ParamEstimatorML<> param_estimator2(param_estimator);
      std::vector< double > v0 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT(v0 == xr0);

      std::vector< double > v1 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT(v1 == xr1);

      std::vector< double > v2 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT(v2 == xr2);

      std::vector< double > v02 =
        param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(v02 == xr02);

      std::vector< double > v01 =
        param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT(v01 == xr01);

      std::vector< double > v21 =
        param_estimator2.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT(v21 == xr21);


      gum::learning::ParamEstimatorML<> param_estimator3(
        std::move(param_estimator2));
      std::vector< double > w0 = param_estimator3.parameters(gum::NodeId(0));
      TS_ASSERT(w0 == xr0);

      std::vector< double > w1 = param_estimator3.parameters(gum::NodeId(1));
      TS_ASSERT(w1 == xr1);

      std::vector< double > w2 = param_estimator3.parameters(gum::NodeId(2));
      TS_ASSERT(w2 == xr2);

      std::vector< double > w02 =
        param_estimator3.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(w02 == xr02);

      std::vector< double > w01 =
        param_estimator3.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT(w01 == xr01);

      std::vector< double > w21 =
        param_estimator3.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT(w21 == xr21);


      gum::learning::ParamEstimatorML<>* param_estimator4 =
        param_estimator.clone();
      std::vector< double > x0 = param_estimator4->parameters(gum::NodeId(0));
      TS_ASSERT(x0 == xr0);

      std::vector< double > x1 = param_estimator4->parameters(gum::NodeId(1));
      TS_ASSERT(x1 == xr1);

      std::vector< double > x2 = param_estimator4->parameters(gum::NodeId(2));
      TS_ASSERT(x2 == xr2);

      std::vector< double > x02 =
        param_estimator4->parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(x02 == xr02);

      std::vector< double > x01 =
        param_estimator4->parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT(x01 == xr01);

      std::vector< double > x21 =
        param_estimator4->parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT(x21 == xr21);

      delete param_estimator4;
    }


    void test3() {
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

      gum::Bijection< gum::NodeId, std::size_t > nodeId2columns;
      nodeId2columns.insert(0, 2);
      nodeId2columns.insert(1, 0);
      nodeId2columns.insert(2, 1);

      gum::learning::ParamEstimatorML<> param_estimator(
        parser, extern_apriori, intern_apriori, nodeId2columns);

      std::vector< double > v0 = param_estimator.parameters(gum::NodeId(1));
      std::vector< double > xv0 = __normalize({1201, 126, 76});
      TS_ASSERT(v0 == xv0);

      std::vector< double > v1 = param_estimator.parameters(gum::NodeId(2));
      std::vector< double > xv1 = __normalize({276, 1076, 51});
      TS_ASSERT(v1 == xv1);

      std::vector< double > v2 = param_estimator.parameters(gum::NodeId(0));
      std::vector< double > xv2 = __normalize({1401, 1, 1});
      TS_ASSERT(v2 == xv2);

      std::vector< double > v02 =
        param_estimator.parameters(gum::NodeId(1), {gum::NodeId(0)});
      std::vector< double > xv02 = __xnormalize({1201, 126, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT(v02 == xv02);

      std::vector< double > v01 =
        param_estimator.parameters(gum::NodeId(1), {gum::NodeId(2)});
      std::vector< double > xv01 =
        __xnormalize({201, 76, 1, 1001, 1, 76, 1, 51, 1});
      TS_ASSERT(v01 == xv01);

      std::vector< double > v21 =
        param_estimator.parameters(gum::NodeId(0), {gum::NodeId(2)});
      std::vector< double > xv21 = __xnormalize({276, 1, 1, 1076, 1, 1, 51, 1, 1});
      TS_ASSERT(v21 == xv21);


      gum::learning::ParamEstimatorML<> param_estimator2(param_estimator);
      std::vector< double > w0 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT(w0 == xv0);

      std::vector< double > w1 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT(w1 == xv1);

      std::vector< double > w2 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT(w2 == xv2);

      std::vector< double > w02 =
        param_estimator2.parameters(gum::NodeId(1), {gum::NodeId(0)});
      TS_ASSERT(w02 == xv02);

      std::vector< double > w01 =
        param_estimator2.parameters(gum::NodeId(1), {gum::NodeId(2)});
      TS_ASSERT(w01 == xv01);

      std::vector< double > w21 =
        param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(w21 == xv21);


      gum::learning::ParamEstimatorML<> param_estimator3(
        std::move(param_estimator2));
      std::vector< double > x0 = param_estimator3.parameters(gum::NodeId(1));
      TS_ASSERT(x0 == xv0);

      std::vector< double > x1 = param_estimator3.parameters(gum::NodeId(2));
      TS_ASSERT(x1 == xv1);

      std::vector< double > x2 = param_estimator3.parameters(gum::NodeId(0));
      TS_ASSERT(x2 == xv2);

      std::vector< double > x02 =
        param_estimator3.parameters(gum::NodeId(1), {gum::NodeId(0)});
      TS_ASSERT(x02 == xv02);

      std::vector< double > x01 =
        param_estimator3.parameters(gum::NodeId(1), {gum::NodeId(2)});
      TS_ASSERT(x01 == xv01);

      std::vector< double > x21 =
        param_estimator3.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(x21 == xv21);


      gum::learning::ParamEstimatorML<>* param_estimator4 =
        param_estimator.clone();
      std::vector< double > y0 = param_estimator4->parameters(gum::NodeId(1));
      TS_ASSERT(y0 == xv0);

      std::vector< double > y1 = param_estimator4->parameters(gum::NodeId(2));
      TS_ASSERT(y1 == xv1);

      std::vector< double > y2 = param_estimator4->parameters(gum::NodeId(0));
      TS_ASSERT(y2 == xv2);

      std::vector< double > y02 =
        param_estimator4->parameters(gum::NodeId(1), {gum::NodeId(0)});
      TS_ASSERT(y02 == xv02);

      std::vector< double > y01 =
        param_estimator4->parameters(gum::NodeId(1), {gum::NodeId(2)});
      TS_ASSERT(y01 == xv01);

      std::vector< double > y21 =
        param_estimator4->parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(y21 == xv21);

      delete param_estimator4;
    }


    void test4() {
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

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000},
                                                                  {1050, 1400}};

      gum::Bijection< gum::NodeId, std::size_t > nodeId2columns;
      nodeId2columns.insert(0, 2);
      nodeId2columns.insert(1, 0);
      nodeId2columns.insert(2, 1);

      gum::learning::ParamEstimatorML<> param_estimator(
        parser, extern_apriori, intern_apriori, ranges, nodeId2columns);

      std::vector< double > r0 = param_estimator.parameters(gum::NodeId(1));
      std::vector< double > xr0 = __normalize({401, 76, 76});
      TS_ASSERT(r0 == xr0);

      std::vector< double > r1 = param_estimator.parameters(gum::NodeId(2));
      std::vector< double > xr1 = __normalize({276, 276, 1});
      TS_ASSERT(r1 == xr1);

      std::vector< double > r2 = param_estimator.parameters(gum::NodeId(0));
      std::vector< double > xr2 = __normalize({551, 1, 1});
      TS_ASSERT(r2 == xr2);

      std::vector< double > r02 =
        param_estimator.parameters(gum::NodeId(1), {gum::NodeId(0)});
      std::vector< double > xr02 = __xnormalize({401, 76, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT(r02 == xr02);

      std::vector< double > r01 =
        param_estimator.parameters(gum::NodeId(1), {gum::NodeId(2)});
      std::vector< double > xr01 = __xnormalize({201, 76, 1, 201, 1, 76, 1, 1, 1});
      TS_ASSERT(r01 == xr01);

      std::vector< double > r21 =
        param_estimator.parameters(gum::NodeId(0), {gum::NodeId(2)});
      std::vector< double > xr21 = __xnormalize({276, 1, 1, 276, 1, 1, 1, 1, 1});
      TS_ASSERT(r21 == xr21);


      gum::learning::ParamEstimatorML<> param_estimator2(param_estimator);
      std::vector< double > v0 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT(v0 == xr0);

      std::vector< double > v1 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT(v1 == xr1);

      std::vector< double > v2 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT(v2 == xr2);

      std::vector< double > v02 =
        param_estimator2.parameters(gum::NodeId(1), {gum::NodeId(0)});
      TS_ASSERT(v02 == xr02);

      std::vector< double > v01 =
        param_estimator2.parameters(gum::NodeId(1), {gum::NodeId(2)});
      TS_ASSERT(v01 == xr01);

      std::vector< double > v21 =
        param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(v21 == xr21);


      gum::learning::ParamEstimatorML<> param_estimator3(
        std::move(param_estimator2));
      std::vector< double > w0 = param_estimator3.parameters(gum::NodeId(1));
      TS_ASSERT(w0 == xr0);

      std::vector< double > w1 = param_estimator3.parameters(gum::NodeId(2));
      TS_ASSERT(w1 == xr1);

      std::vector< double > w2 = param_estimator3.parameters(gum::NodeId(0));
      TS_ASSERT(w2 == xr2);

      std::vector< double > w02 =
        param_estimator3.parameters(gum::NodeId(1), {gum::NodeId(0)});
      TS_ASSERT(w02 == xr02);

      std::vector< double > w01 =
        param_estimator3.parameters(gum::NodeId(1), {gum::NodeId(2)});
      TS_ASSERT(w01 == xr01);

      std::vector< double > w21 =
        param_estimator3.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(w21 == xr21);


      gum::learning::ParamEstimatorML<>* param_estimator4 =
        param_estimator.clone();
      std::vector< double > x0 = param_estimator4->parameters(gum::NodeId(1));
      TS_ASSERT(x0 == xr0);

      std::vector< double > x1 = param_estimator4->parameters(gum::NodeId(2));
      TS_ASSERT(x1 == xr1);

      std::vector< double > x2 = param_estimator4->parameters(gum::NodeId(0));
      TS_ASSERT(x2 == xr2);

      std::vector< double > x02 =
        param_estimator4->parameters(gum::NodeId(1), {gum::NodeId(0)});
      TS_ASSERT(x02 == xr02);

      std::vector< double > x01 =
        param_estimator4->parameters(gum::NodeId(1), {gum::NodeId(2)});
      TS_ASSERT(x01 == xr01);

      std::vector< double > x21 =
        param_estimator4->parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(x21 == xr21);

      delete param_estimator4;
    }

    void testChangeRanges() {
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

      std::vector< double > v0 = param_estimator.parameters(gum::NodeId(0));
      std::vector< double > xv0 = __normalize({1201, 126, 76});
      TS_ASSERT(v0 == xv0);

      std::vector< double > v1 = param_estimator.parameters(gum::NodeId(1));
      std::vector< double > xv1 = __normalize({276, 1076, 51});
      TS_ASSERT(v1 == xv1);

      std::vector< double > v2 = param_estimator.parameters(gum::NodeId(2));
      std::vector< double > xv2 = __normalize({1401, 1, 1});
      TS_ASSERT(v2 == xv2);

      std::vector< double > v02 =
        param_estimator.parameters(gum::NodeId(0), {gum::NodeId(2)});
      std::vector< double > xv02 = __xnormalize({1201, 126, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT(v02 == xv02);

      std::vector< double > v01 =
        param_estimator.parameters(gum::NodeId(0), {gum::NodeId(1)});
      std::vector< double > xv01 =
        __xnormalize({201, 76, 1, 1001, 1, 76, 1, 51, 1});
      TS_ASSERT(v01 == xv01);

      std::vector< double > v21 =
        param_estimator.parameters(gum::NodeId(2), {gum::NodeId(1)});
      std::vector< double > xv21 = __xnormalize({276, 1, 1, 1076, 1, 1, 51, 1, 1});
      TS_ASSERT(v21 == xv21);


      gum::learning::ParamEstimatorML<> param_estimator2(param_estimator);
      std::vector< double > w0 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT(w0 == xv0);

      std::vector< double > w1 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT(w1 == xv1);

      std::vector< double > w2 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT(w2 == xv2);

      std::vector< double > w02 =
        param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(w02 == xv02);

      std::vector< double > w01 =
        param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT(w01 == xv01);

      std::vector< double > w21 =
        param_estimator2.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT(w21 == xv21);

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000},
                                                                  {1050, 1400}};
      param_estimator.setRanges(ranges);

      std::vector< double > ar0 = param_estimator.parameters(gum::NodeId(0));
      std::vector< double > axr0 = __normalize({401, 76, 76});
      TS_ASSERT(ar0 == axr0);

      std::vector< double > ar1 = param_estimator.parameters(gum::NodeId(1));
      std::vector< double > axr1 = __normalize({276, 276, 1});
      TS_ASSERT(ar1 == axr1);

      std::vector< double > ar2 = param_estimator.parameters(gum::NodeId(2));
      std::vector< double > axr2 = __normalize({551, 1, 1});
      TS_ASSERT(ar2 == axr2);

      std::vector< double > ar02 =
        param_estimator.parameters(gum::NodeId(0), {gum::NodeId(2)});
      std::vector< double > axr02 = __xnormalize({401, 76, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT(ar02 == axr02);

      std::vector< double > ar01 =
        param_estimator.parameters(gum::NodeId(0), {gum::NodeId(1)});
      std::vector< double > axr01 =
        __xnormalize({201, 76, 1, 201, 1, 76, 1, 1, 1});
      TS_ASSERT(ar01 == axr01);

      std::vector< double > ar21 =
        param_estimator.parameters(gum::NodeId(2), {gum::NodeId(1)});
      std::vector< double > axr21 = __xnormalize({276, 1, 1, 276, 1, 1, 1, 1, 1});
      TS_ASSERT(ar21 == axr21);

      param_estimator2.setRanges(ranges);

      std::vector< double > av0 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT(av0 == axr0);

      std::vector< double > av1 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT(av1 == axr1);

      std::vector< double > av2 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT(av2 == axr2);

      std::vector< double > av02 =
        param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(av02 == axr02);

      std::vector< double > av01 =
        param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT(av01 == axr01);

      std::vector< double > av21 =
        param_estimator2.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT(av21 == axr21);

      TS_ASSERT(param_estimator2.ranges() == ranges);

      param_estimator2.clearRanges();

      std::vector< double > bv0 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT(bv0 == xv0);

      std::vector< double > bv1 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT(bv1 == xv1);

      std::vector< double > bv2 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT(bv2 == xv2);

      std::vector< double > bv02 =
        param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT(bv02 == xv02);

      std::vector< double > bv01 =
        param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT(bv01 == xv01);

      std::vector< double > bv21 =
        param_estimator2.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT(bv21 == xv21);
    }


    void testEM () {
      gum::LabelizedVariable var("x", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      const std::vector<std::string> miss {"N/A","?"};
      gum::learning::DBTranslator4LabelizedVariable<> translator(var,miss);
      gum::learning::DBTranslatorSet<> set;
      for ( std::size_t i = std::size_t(0); i < std::size_t(4); ++i)
        set.insertTranslator ( translator, i );

      set[0].setVariableName ( "A" );
      set[1].setVariableName ( "B" );
      set[2].setVariableName ( "C" );
      set[3].setVariableName ( "D" );

      gum::learning::DatabaseTable<> database ( set );
      std::vector<std::string> row1 { "0", "1", "1", "0" };
      std::vector<std::string> row2 { "0", "?", "1", "0" };
      std::vector<std::string> row3 { "0", "?", "?", "0" };
      std::vector<std::string> row4 { "?", "?", "1", "0" };
      std::vector<std::string> row5 { "?", "?", "?", "?" };
      for (int i = 0; i < 100; ++i) {
        database.insertRow( row1 );
        database.insertRow( row2 );
        database.insertRow( row3 );
        database.insertRow( row4 );
        database.insertRow( row5 );
      }

      const std::vector< gum::learning::DBTranslatedValueType > col_types{
        gum::learning::DBTranslatedValueType::DISCRETE,
        gum::learning::DBTranslatedValueType::DISCRETE,
        gum::learning::DBTranslatedValueType::DISCRETE,
        gum::learning::DBTranslatedValueType::DISCRETE};

      auto bn0 = gum::BayesNet< double >::fastPrototype("A;B;C;D");
      bn0.cpt("A").fillWith({0.3, 0.7});
      bn0.cpt("B").fillWith({0.3, 0.7});
      bn0.cpt("C").fillWith({0.3, 0.7});
      bn0.cpt("D").fillWith({0.3, 0.7});
      
      gum::learning::DBRowGeneratorIdentity<> generator1(col_types);
      gum::learning::DBRowGeneratorEM<>       generator2(col_types,bn0);
      gum::learning::DBRowGeneratorIdentity<> generator3(col_types);
      gum::learning::DBRowGeneratorEM<>       generator4(col_types,bn0);

      gum::learning::DBRowGeneratorSet<> genset;
      genset.insertGenerator(generator1);
      genset.insertGenerator(generator2);
      genset.insertGenerator(generator3);
      genset.insertGenerator(generator4);

      gum::learning::DBRowGeneratorParser<> parser ( database.handler (), genset );
      gum::learning::AprioriNoApriori<>     extern_apriori(database);
      gum::learning::AprioriNoApriori<>     intern_apriori(database);

      gum::learning::ParamEstimatorML<> param_estimator(
        parser, extern_apriori, intern_apriori);

      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");
      bn.cpt("A").fillWith({0.3, 0.7});
      bn.cpt("B").fillWith({0.4, 0.6, 0.7, 0.3});
      bn.cpt("C").fillWith({0.2, 0.8, 0.3, 0.7, 0.4, 0.6, 0.5, 0.5});
      bn.cpt("D").fillWith({0.8, 0.2});

      // bugfix for parallel exceution of VariableElimination
      const gum::DAG& dag = bn.dag();
      for ( const auto node : dag ) {
        dag.parents(node);
        dag.children(node);
      }

      param_estimator.setBayesNet(bn);
      
      gum::learning::IdSet<> ids(0, std::vector< gum::NodeId >{1}, true);
      //gum::learning::IdSet<> ids(0, {}, true);
      std::vector< double >  counts =
        param_estimator.parameters(gum::NodeId(0), {gum::NodeId(1)});
           
      std::vector< double >  xcounts(4,0.0);
      std::vector< double >  sum(4,0.0);
      int nb_row = 0;
      for(const auto& row : database ) {
        gum::Potential<double> proba =
          __infer(bn, {std::size_t(0),std::size_t(1)}, row);

        std::size_t idx;
        for (gum::Instantiation inst(proba); !inst.end(); ++inst) {
          if (proba.variablesSequence()[0]->name() == "A") 
            idx = inst.val(0) + std::size_t(2) * inst.val(1);
          else
            idx = inst.val(1) + std::size_t(2) * inst.val(0);
          xcounts[idx] += proba.get(inst);
        }

        ++nb_row;
        if ( nb_row == 5) break;
      }
      
      sum[0] = xcounts[0] + xcounts[1];
      sum[1] = sum[0];
      sum[2] = xcounts[2] + xcounts[3];
      sum[3] = sum[2];
     
      for (std::size_t i = std::size_t(0); i < std::size_t(4); ++i)
        xcounts[i] /= sum[i];

      for ( std::size_t i = std::size_t(0); i < std::size_t(4); ++i) {
        TS_ASSERT_DELTA(counts[i], xcounts[i], 0.001);
      }
    }

  };

}   // namespace gum_tests
