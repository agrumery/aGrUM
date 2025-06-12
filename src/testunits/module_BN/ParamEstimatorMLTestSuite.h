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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/DBRowGeneratorEM.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>
#include <agrum/BN/learning/priors/noPrior.h>
#include <agrum/BN/learning/priors/smoothingPrior.h>

namespace gum_tests {

  class [[maybe_unused]] ParamEstimatorMLTestSuite: public CxxTest::TestSuite {
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

    gum::Tensor< double >
        _infer_(const gum::BayesNet< double >&                                  bn,
                const std::vector< std::size_t >&                               targets,
                const gum::learning::DBRow< gum::learning::DBTranslatedValue >& row) {
      gum::LazyPropagation< double > ve(&bn);

      gum::NodeSet target_set;
      for (auto target: targets)
        target_set.insert(gum::NodeId(target));
      ve.addJointTarget(target_set);

      const auto xrow     = row.row();
      const auto row_size = xrow.size();
      for (std::size_t col = std::size_t(0); col < row_size; ++col) {
        if (xrow[col].discr_val != std::numeric_limits< std::size_t >::max()) {
          ve.addEvidence(gum::NodeId(col), xrow[col].discr_val);
        }
      }

      gum::Tensor< double > prob = ve.jointPosterior(target_set);
      return prob;
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

      std::vector< double > v0  = param_estimator.parameters(gum::NodeId(0));
      std::vector< double > xv0 = _normalize_({1201, 126, 76});
      TS_ASSERT_EQUALS(v0, xv0)

      std::vector< double > v1  = param_estimator.parameters(gum::NodeId(1));
      std::vector< double > xv1 = _normalize_({276, 1076, 51});
      TS_ASSERT_EQUALS(v1, xv1)

      std::vector< double > v2  = param_estimator.parameters(gum::NodeId(2));
      std::vector< double > xv2 = _normalize_({1401, 1, 1});
      TS_ASSERT_EQUALS(v2, xv2)

      std::vector< double > v02  = param_estimator.parameters(gum::NodeId(0), {gum::NodeId(2)});
      std::vector< double > xv02 = _xnormalize_({1201, 126, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT_EQUALS(v02, xv02)

      std::vector< double > v01  = param_estimator.parameters(gum::NodeId(0), {gum::NodeId(1)});
      std::vector< double > xv01 = _xnormalize_({201, 76, 1, 1001, 1, 76, 1, 51, 1});
      TS_ASSERT_EQUALS(v01, xv01)

      std::vector< double > v21  = param_estimator.parameters(gum::NodeId(2), {gum::NodeId(1)});
      std::vector< double > xv21 = _xnormalize_({276, 1, 1, 1076, 1, 1, 51, 1, 1});
      TS_ASSERT_EQUALS(v21, xv21)


      gum::learning::ParamEstimatorML param_estimator2(param_estimator);
      std::vector< double >           w0 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(w0, xv0)

      std::vector< double > w1 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(w1, xv1)

      std::vector< double > w2 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(w2, xv2)

      std::vector< double > w02 = param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(w02, xv02)

      std::vector< double > w01 = param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(w01, xv01)

      std::vector< double > w21 = param_estimator2.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(w21, xv21)


      gum::learning::ParamEstimatorML param_estimator3(std::move(param_estimator2));
      std::vector< double >           x0 = param_estimator3.parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(x0, xv0)

      std::vector< double > x1 = param_estimator3.parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(x1, xv1)

      std::vector< double > x2 = param_estimator3.parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(x2, xv2)

      std::vector< double > x02 = param_estimator3.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(x02, xv02)

      std::vector< double > x01 = param_estimator3.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(x01, xv01)

      std::vector< double > x21 = param_estimator3.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(x21, xv21)


      gum::learning::ParamEstimatorML* param_estimator4 = param_estimator.clone();
      std::vector< double >            y0 = param_estimator4->parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(y0, xv0)

      std::vector< double > y1 = param_estimator4->parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(y1, xv1)

      std::vector< double > y2 = param_estimator4->parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(y2, xv2)

      std::vector< double > y02 = param_estimator4->parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(y02, xv02)

      std::vector< double > y01 = param_estimator4->parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(y01, xv01)

      std::vector< double > y21 = param_estimator4->parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(y21, xv21)

      delete param_estimator4;
    }

    GUM_ACTIVE_TEST(2) {
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

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000}, {1050, 1400}};

      gum::learning::ParamEstimatorML param_estimator(parser, extern_prior, intern_prior, ranges);

      std::vector< double > r0  = param_estimator.parameters(gum::NodeId(0));
      std::vector< double > xr0 = _normalize_({401, 76, 76});
      TS_ASSERT_EQUALS(r0, xr0)

      std::vector< double > r1  = param_estimator.parameters(gum::NodeId(1));
      std::vector< double > xr1 = _normalize_({276, 276, 1});
      TS_ASSERT_EQUALS(r1, xr1)

      std::vector< double > r2  = param_estimator.parameters(gum::NodeId(2));
      std::vector< double > xr2 = _normalize_({551, 1, 1});
      TS_ASSERT_EQUALS(r2, xr2)

      std::vector< double > r02  = param_estimator.parameters(gum::NodeId(0), {gum::NodeId(2)});
      std::vector< double > xr02 = _xnormalize_({401, 76, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT_EQUALS(r02, xr02)

      std::vector< double > r01  = param_estimator.parameters(gum::NodeId(0), {gum::NodeId(1)});
      std::vector< double > xr01 = _xnormalize_({201, 76, 1, 201, 1, 76, 1, 1, 1});
      TS_ASSERT_EQUALS(r01, xr01)

      std::vector< double > r21  = param_estimator.parameters(gum::NodeId(2), {gum::NodeId(1)});
      std::vector< double > xr21 = _xnormalize_({276, 1, 1, 276, 1, 1, 1, 1, 1});
      TS_ASSERT_EQUALS(r21, xr21)


      gum::learning::ParamEstimatorML param_estimator2(param_estimator);
      std::vector< double >           v0 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(v0, xr0)

      std::vector< double > v1 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(v1, xr1)

      std::vector< double > v2 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(v2, xr2)

      std::vector< double > v02 = param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(v02, xr02)

      std::vector< double > v01 = param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(v01, xr01)

      std::vector< double > v21 = param_estimator2.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(v21, xr21)


      gum::learning::ParamEstimatorML param_estimator3(std::move(param_estimator2));
      std::vector< double >           w0 = param_estimator3.parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(w0, xr0)

      std::vector< double > w1 = param_estimator3.parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(w1, xr1)

      std::vector< double > w2 = param_estimator3.parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(w2, xr2)

      std::vector< double > w02 = param_estimator3.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(w02, xr02)

      std::vector< double > w01 = param_estimator3.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(w01, xr01)

      std::vector< double > w21 = param_estimator3.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(w21, xr21)


      gum::learning::ParamEstimatorML* param_estimator4 = param_estimator.clone();
      std::vector< double >            x0 = param_estimator4->parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(x0, xr0)

      std::vector< double > x1 = param_estimator4->parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(x1, xr1)

      std::vector< double > x2 = param_estimator4->parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(x2, xr2)

      std::vector< double > x02 = param_estimator4->parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(x02, xr02)

      std::vector< double > x01 = param_estimator4->parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(x01, xr01)

      std::vector< double > x21 = param_estimator4->parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(x21, xr21)

      delete param_estimator4;
    }

    GUM_ACTIVE_TEST(3) {
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

      gum::Bijection< gum::NodeId, std::size_t > nodeId2columns;
      nodeId2columns.insert(0, 2);
      nodeId2columns.insert(1, 0);
      nodeId2columns.insert(2, 1);

      gum::learning::ParamEstimatorML param_estimator(parser,
                                                      extern_prior,
                                                      intern_prior,
                                                      nodeId2columns);

      std::vector< double > v0  = param_estimator.parameters(gum::NodeId(1));
      std::vector< double > xv0 = _normalize_({1201, 126, 76});
      TS_ASSERT_EQUALS(v0, xv0)

      std::vector< double > v1  = param_estimator.parameters(gum::NodeId(2));
      std::vector< double > xv1 = _normalize_({276, 1076, 51});
      TS_ASSERT_EQUALS(v1, xv1)

      std::vector< double > v2  = param_estimator.parameters(gum::NodeId(0));
      std::vector< double > xv2 = _normalize_({1401, 1, 1});
      TS_ASSERT_EQUALS(v2, xv2)

      std::vector< double > v02  = param_estimator.parameters(gum::NodeId(1), {gum::NodeId(0)});
      std::vector< double > xv02 = _xnormalize_({1201, 126, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT_EQUALS(v02, xv02)

      std::vector< double > v01  = param_estimator.parameters(gum::NodeId(1), {gum::NodeId(2)});
      std::vector< double > xv01 = _xnormalize_({201, 76, 1, 1001, 1, 76, 1, 51, 1});
      TS_ASSERT_EQUALS(v01, xv01)

      std::vector< double > v21  = param_estimator.parameters(gum::NodeId(0), {gum::NodeId(2)});
      std::vector< double > xv21 = _xnormalize_({276, 1, 1, 1076, 1, 1, 51, 1, 1});
      TS_ASSERT_EQUALS(v21, xv21)


      gum::learning::ParamEstimatorML param_estimator2(param_estimator);
      std::vector< double >           w0 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(w0, xv0)

      std::vector< double > w1 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(w1, xv1)

      std::vector< double > w2 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(w2, xv2)

      std::vector< double > w02 = param_estimator2.parameters(gum::NodeId(1), {gum::NodeId(0)});
      TS_ASSERT_EQUALS(w02, xv02)

      std::vector< double > w01 = param_estimator2.parameters(gum::NodeId(1), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(w01, xv01)

      std::vector< double > w21 = param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(w21, xv21)


      gum::learning::ParamEstimatorML param_estimator3(std::move(param_estimator2));
      std::vector< double >           x0 = param_estimator3.parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(x0, xv0)

      std::vector< double > x1 = param_estimator3.parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(x1, xv1)

      std::vector< double > x2 = param_estimator3.parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(x2, xv2)

      std::vector< double > x02 = param_estimator3.parameters(gum::NodeId(1), {gum::NodeId(0)});
      TS_ASSERT_EQUALS(x02, xv02)

      std::vector< double > x01 = param_estimator3.parameters(gum::NodeId(1), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(x01, xv01)

      std::vector< double > x21 = param_estimator3.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(x21, xv21)


      gum::learning::ParamEstimatorML* param_estimator4 = param_estimator.clone();
      std::vector< double >            y0 = param_estimator4->parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(y0, xv0)

      std::vector< double > y1 = param_estimator4->parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(y1, xv1)

      std::vector< double > y2 = param_estimator4->parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(y2, xv2)

      std::vector< double > y02 = param_estimator4->parameters(gum::NodeId(1), {gum::NodeId(0)});
      TS_ASSERT_EQUALS(y02, xv02)

      std::vector< double > y01 = param_estimator4->parameters(gum::NodeId(1), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(y01, xv01)

      std::vector< double > y21 = param_estimator4->parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(y21, xv21)

      delete param_estimator4;
    }

    GUM_ACTIVE_TEST(4) {
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

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000}, {1050, 1400}};

      gum::Bijection< gum::NodeId, std::size_t > nodeId2columns;
      nodeId2columns.insert(0, 2);
      nodeId2columns.insert(1, 0);
      nodeId2columns.insert(2, 1);

      gum::learning::ParamEstimatorML param_estimator(parser,
                                                      extern_prior,
                                                      intern_prior,
                                                      ranges,
                                                      nodeId2columns);

      std::vector< double > r0  = param_estimator.parameters(gum::NodeId(1));
      std::vector< double > xr0 = _normalize_({401, 76, 76});
      TS_ASSERT_EQUALS(r0, xr0)

      std::vector< double > r1  = param_estimator.parameters(gum::NodeId(2));
      std::vector< double > xr1 = _normalize_({276, 276, 1});
      TS_ASSERT_EQUALS(r1, xr1)

      std::vector< double > r2  = param_estimator.parameters(gum::NodeId(0));
      std::vector< double > xr2 = _normalize_({551, 1, 1});
      TS_ASSERT_EQUALS(r2, xr2)

      std::vector< double > r02  = param_estimator.parameters(gum::NodeId(1), {gum::NodeId(0)});
      std::vector< double > xr02 = _xnormalize_({401, 76, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT_EQUALS(r02, xr02)

      std::vector< double > r01  = param_estimator.parameters(gum::NodeId(1), {gum::NodeId(2)});
      std::vector< double > xr01 = _xnormalize_({201, 76, 1, 201, 1, 76, 1, 1, 1});
      TS_ASSERT_EQUALS(r01, xr01)

      std::vector< double > r21  = param_estimator.parameters(gum::NodeId(0), {gum::NodeId(2)});
      std::vector< double > xr21 = _xnormalize_({276, 1, 1, 276, 1, 1, 1, 1, 1});
      TS_ASSERT_EQUALS(r21, xr21)


      gum::learning::ParamEstimatorML param_estimator2(param_estimator);
      std::vector< double >           v0 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(v0, xr0)

      std::vector< double > v1 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(v1, xr1)

      std::vector< double > v2 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(v2, xr2)

      std::vector< double > v02 = param_estimator2.parameters(gum::NodeId(1), {gum::NodeId(0)});
      TS_ASSERT_EQUALS(v02, xr02)

      std::vector< double > v01 = param_estimator2.parameters(gum::NodeId(1), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(v01, xr01)

      std::vector< double > v21 = param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(v21, xr21)


      gum::learning::ParamEstimatorML param_estimator3(std::move(param_estimator2));
      std::vector< double >           w0 = param_estimator3.parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(w0, xr0)

      std::vector< double > w1 = param_estimator3.parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(w1, xr1)

      std::vector< double > w2 = param_estimator3.parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(w2, xr2)

      std::vector< double > w02 = param_estimator3.parameters(gum::NodeId(1), {gum::NodeId(0)});
      TS_ASSERT_EQUALS(w02, xr02)

      std::vector< double > w01 = param_estimator3.parameters(gum::NodeId(1), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(w01, xr01)

      std::vector< double > w21 = param_estimator3.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(w21, xr21)


      gum::learning::ParamEstimatorML* param_estimator4 = param_estimator.clone();
      std::vector< double >            x0 = param_estimator4->parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(x0, xr0)

      std::vector< double > x1 = param_estimator4->parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(x1, xr1)

      std::vector< double > x2 = param_estimator4->parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(x2, xr2)

      std::vector< double > x02 = param_estimator4->parameters(gum::NodeId(1), {gum::NodeId(0)});
      TS_ASSERT_EQUALS(x02, xr02)

      std::vector< double > x01 = param_estimator4->parameters(gum::NodeId(1), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(x01, xr01)

      std::vector< double > x21 = param_estimator4->parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(x21, xr21)

      delete param_estimator4;
    }

    GUM_ACTIVE_TEST(ChangeRanges) {
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

      std::vector< double > v0  = param_estimator.parameters(gum::NodeId(0));
      std::vector< double > xv0 = _normalize_({1201, 126, 76});
      TS_ASSERT_EQUALS(v0, xv0)

      std::vector< double > v1  = param_estimator.parameters(gum::NodeId(1));
      std::vector< double > xv1 = _normalize_({276, 1076, 51});
      TS_ASSERT_EQUALS(v1, xv1)

      std::vector< double > v2  = param_estimator.parameters(gum::NodeId(2));
      std::vector< double > xv2 = _normalize_({1401, 1, 1});
      TS_ASSERT_EQUALS(v2, xv2)

      std::vector< double > v02  = param_estimator.parameters(gum::NodeId(0), {gum::NodeId(2)});
      std::vector< double > xv02 = _xnormalize_({1201, 126, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT_EQUALS(v02, xv02)

      std::vector< double > v01  = param_estimator.parameters(gum::NodeId(0), {gum::NodeId(1)});
      std::vector< double > xv01 = _xnormalize_({201, 76, 1, 1001, 1, 76, 1, 51, 1});
      TS_ASSERT_EQUALS(v01, xv01)

      std::vector< double > v21  = param_estimator.parameters(gum::NodeId(2), {gum::NodeId(1)});
      std::vector< double > xv21 = _xnormalize_({276, 1, 1, 1076, 1, 1, 51, 1, 1});
      TS_ASSERT_EQUALS(v21, xv21)


      gum::learning::ParamEstimatorML param_estimator2(param_estimator);
      std::vector< double >           w0 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(w0, xv0)

      std::vector< double > w1 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(w1, xv1)

      std::vector< double > w2 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(w2, xv2)

      std::vector< double > w02 = param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(w02, xv02)

      std::vector< double > w01 = param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(w01, xv01)

      std::vector< double > w21 = param_estimator2.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(w21, xv21)

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000}, {1050, 1400}};
      param_estimator.setRanges(ranges);

      std::vector< double > ar0  = param_estimator.parameters(gum::NodeId(0));
      std::vector< double > axr0 = _normalize_({401, 76, 76});
      TS_ASSERT_EQUALS(ar0, axr0)

      std::vector< double > ar1  = param_estimator.parameters(gum::NodeId(1));
      std::vector< double > axr1 = _normalize_({276, 276, 1});
      TS_ASSERT_EQUALS(ar1, axr1)

      std::vector< double > ar2  = param_estimator.parameters(gum::NodeId(2));
      std::vector< double > axr2 = _normalize_({551, 1, 1});
      TS_ASSERT_EQUALS(ar2, axr2)

      std::vector< double > ar02  = param_estimator.parameters(gum::NodeId(0), {gum::NodeId(2)});
      std::vector< double > axr02 = _xnormalize_({401, 76, 76, 1, 1, 1, 1, 1, 1});
      TS_ASSERT_EQUALS(ar02, axr02)

      std::vector< double > ar01  = param_estimator.parameters(gum::NodeId(0), {gum::NodeId(1)});
      std::vector< double > axr01 = _xnormalize_({201, 76, 1, 201, 1, 76, 1, 1, 1});
      TS_ASSERT_EQUALS(ar01, axr01)

      std::vector< double > ar21  = param_estimator.parameters(gum::NodeId(2), {gum::NodeId(1)});
      std::vector< double > axr21 = _xnormalize_({276, 1, 1, 276, 1, 1, 1, 1, 1});
      TS_ASSERT_EQUALS(ar21, axr21)

      param_estimator2.setRanges(ranges);

      std::vector< double > av0 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(av0, axr0)

      std::vector< double > av1 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(av1, axr1)

      std::vector< double > av2 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(av2, axr2)

      std::vector< double > av02 = param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(av02, axr02)

      std::vector< double > av01 = param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(av01, axr01)

      std::vector< double > av21 = param_estimator2.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(av21, axr21)

      TS_ASSERT_EQUALS(param_estimator2.ranges(), ranges)

      param_estimator2.clearRanges();

      std::vector< double > bv0 = param_estimator2.parameters(gum::NodeId(0));
      TS_ASSERT_EQUALS(bv0, xv0)

      std::vector< double > bv1 = param_estimator2.parameters(gum::NodeId(1));
      TS_ASSERT_EQUALS(bv1, xv1)

      std::vector< double > bv2 = param_estimator2.parameters(gum::NodeId(2));
      TS_ASSERT_EQUALS(bv2, xv2)

      std::vector< double > bv02 = param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(2)});
      TS_ASSERT_EQUALS(bv02, xv02)

      std::vector< double > bv01 = param_estimator2.parameters(gum::NodeId(0), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(bv01, xv01)

      std::vector< double > bv21 = param_estimator2.parameters(gum::NodeId(2), {gum::NodeId(1)});
      TS_ASSERT_EQUALS(bv21, xv21)
    }

    GUM_ACTIVE_TEST(EM) {
      gum::LabelizedVariable var("x", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      const std::vector< std::string >              miss{"N/A", "?"};
      gum::learning::DBTranslator4LabelizedVariable translator(var, miss);
      gum::learning::DBTranslatorSet                set;
      for (std::size_t i = std::size_t(0); i < std::size_t(4); ++i)
        set.insertTranslator(translator, i);

      set[0].setVariableName("A");
      set[1].setVariableName("B");
      set[2].setVariableName("C");
      set[3].setVariableName("D");

      gum::learning::DatabaseTable database(set);
      std::vector< std::string >   row1{"0", "1", "1", "0"};
      std::vector< std::string >   row2{"0", "?", "1", "0"};
      std::vector< std::string >   row3{"0", "?", "?", "0"};
      std::vector< std::string >   row4{"?", "?", "1", "0"};
      std::vector< std::string >   row5{"?", "?", "?", "?"};
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

      auto bn0 = gum::BayesNet< double >::fastPrototype("A;B;C;D");
      bn0.cpt("A").fillWith({0.3, 0.7});
      bn0.cpt("B").fillWith({0.3, 0.7});
      bn0.cpt("C").fillWith({0.3, 0.7});
      bn0.cpt("D").fillWith({0.3, 0.7});

      gum::learning::DBRowGeneratorIdentity generator1(col_types);
      gum::learning::DBRowGeneratorEM       generator2(col_types, bn0);
      gum::learning::DBRowGenerator&        gen2 = generator2;   // fix for g++-4.8
      gum::learning::DBRowGeneratorIdentity generator3(col_types);
      gum::learning::DBRowGeneratorEM       generator4(col_types, bn0);
      gum::learning::DBRowGenerator&        gen4 = generator4;   // fix for g++-4.8

      gum::learning::DBRowGeneratorSet genset;
      genset.insertGenerator(generator1);
      genset.insertGenerator(gen2);
      genset.insertGenerator(generator3);
      genset.insertGenerator(gen4);

      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::NoPrior              extern_prior(database);
      gum::learning::NoPrior              intern_prior(database);

      gum::learning::ParamEstimatorML param_estimator(parser, extern_prior, intern_prior);

      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");
      bn.cpt("A").fillWith({0.3, 0.7});
      bn.cpt("B").fillWith({0.4, 0.6, 0.7, 0.3});
      bn.cpt("C").fillWith({0.2, 0.8, 0.3, 0.7, 0.4, 0.6, 0.5, 0.5});
      bn.cpt("D").fillWith({0.8, 0.2});

      // bugfix for parallel execution of VariableElimination
      const gum::DAG& dag = bn.dag();
      for (const auto node: dag) {
        dag.parents(node);
        dag.children(node);
      }

      param_estimator.setBayesNet(bn);

      gum::learning::IdCondSet ids(0, std::vector< gum::NodeId >{1}, true);
      // gum::learning::IdCondSet ids(0, {}, true);
      std::vector< double > counts = param_estimator.parameters(gum::NodeId(0), {gum::NodeId(1)});

      std::vector< double > xcounts(4, 0.0);
      std::vector< double > sum(4, 0.0);
      int                   nb_row = 0;
      for (const auto& row: database) {
        gum::Tensor< double > proba = _infer_(bn, {std::size_t(0), std::size_t(1)}, row);

        std::size_t idx;
        for (gum::Instantiation inst(proba); !inst.end(); ++inst) {
          if (proba.variablesSequence()[0]->name() == "A")
            idx = inst.val(0) + std::size_t(2) * inst.val(1);
          else idx = inst.val(1) + std::size_t(2) * inst.val(0);
          xcounts[idx] += proba.get(inst);
        }

        ++nb_row;
        if (nb_row == 5) break;
      }

      sum[0] = xcounts[0] + xcounts[1];
      sum[1] = sum[0];
      sum[2] = xcounts[2] + xcounts[3];
      sum[3] = sum[2];

      for (std::size_t i = std::size_t(0); i < std::size_t(4); ++i)
        xcounts[i] /= sum[i];

      for (std::size_t i = std::size_t(0); i < std::size_t(4); ++i) {
        TS_ASSERT_DELTA(counts[i], xcounts[i], 0.001)
      }
    }

    GUM_ACTIVE_TEST(ZeroInPseudoCounts) {
      gum::learning::DBTranslatorSet trans_set;
      {
        // create the translator set
        gum::LabelizedVariable var("X1", "", 0);
        var.addLabel("0");
        var.addLabel("1");
        var.addLabel("2");

        const std::vector< std::string >              miss;
        gum::learning::DBTranslator4LabelizedVariable translator(var, miss);
        std::vector< std::string >                    names{"A", "B"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable database(trans_set);
      database.insertRow({"0", "1"});
      database.insertRow({"1", "0"});
      database.insertRow({"1", "1"});
      database.insertRow({"0", "1"});
      database.insertRow({"0", "0"});

      // create the parser
      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::NoPrior              extern_prior(database);
      gum::learning::NoPrior              intern_prior(database);

      gum::learning::ParamEstimatorML param_estimator(parser, extern_prior, intern_prior);

      TS_GUM_ASSERT_THROWS_NOTHING(param_estimator.parameters(gum::NodeId(0)))
      TS_GUM_ASSERT_THROWS_NOTHING(param_estimator.parameters(gum::NodeId(1)))
      TS_ASSERT_THROWS(param_estimator.parameters(gum::NodeId(1), {gum::NodeId(0)}),
                       const gum::DatabaseError&)
    }

    GUM_ACTIVE_TEST(_multicore) {
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

      for (std::size_t i = std::size_t(1); i < std::size_t(24); ++i) {
        param_estimator.setNumberOfThreads(i);

        std::vector< double > v0  = param_estimator.parameters(gum::NodeId(0));
        std::vector< double > xv0 = _normalize_({1201, 126, 76});
        TS_ASSERT_EQUALS(v0, xv0)

        std::vector< double > v1  = param_estimator.parameters(gum::NodeId(1));
        std::vector< double > xv1 = _normalize_({276, 1076, 51});
        TS_ASSERT_EQUALS(v1, xv1)

        std::vector< double > v2  = param_estimator.parameters(gum::NodeId(2));
        std::vector< double > xv2 = _normalize_({1401, 1, 1});
        TS_ASSERT_EQUALS(v2, xv2)

        std::vector< double > v02  = param_estimator.parameters(gum::NodeId(0), {gum::NodeId(2)});
        std::vector< double > xv02 = _xnormalize_({1201, 126, 76, 1, 1, 1, 1, 1, 1});
        TS_ASSERT_EQUALS(v02, xv02)

        std::vector< double > v01  = param_estimator.parameters(gum::NodeId(0), {gum::NodeId(1)});
        std::vector< double > xv01 = _xnormalize_({201, 76, 1, 1001, 1, 76, 1, 51, 1});
        TS_ASSERT_EQUALS(v01, xv01)

        std::vector< double > v21  = param_estimator.parameters(gum::NodeId(2), {gum::NodeId(1)});
        std::vector< double > xv21 = _xnormalize_({276, 1, 1, 1076, 1, 1, 51, 1, 1});
        TS_ASSERT_EQUALS(v21, xv21)
      }
    }
  };

}   // namespace gum_tests
