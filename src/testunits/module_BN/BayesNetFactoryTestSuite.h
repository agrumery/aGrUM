/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/multidim/implementations/multiDimSparse.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/BayesNetFactory.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  BayesNetFactory
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  struct BayesNetFactoryTestSuite {
    private:
    gum::BayesNet< double >* __bn_d{nullptr};
    // gum::BayesNet<float>*   __bn_f;

    public:
    BayesNetFactoryTestSuite() { setUp(); }

    ~BayesNetFactoryTestSuite() { tearDown(); }

    void setUp() { __bn_d = new gum::BayesNet< double >(); }

    void tearDown() { delete __bn_d; }

    void testCreation() {
      gum::BayesNetFactory< double >* factory = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete factory);
    }

    void testIllegalCallsInNONE() const {
      std::vector< float >            aSequence;
      gum::BayesNetFactory< double >* factory = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d));

      CHECK_THROWS_AS(factory->variableName("foo"), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->variableDescription("bar"), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->variableType(gum::VarType::LABELIZED),
                      const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->addModality("plop"), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->setVariableCPTImplementation(0), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->endVariableDeclaration(), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->addParent("foo"), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->endParentsDeclaration(), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->rawConditionalTable(aSequence), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->endRawProbabilityDeclaration(), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->startFactorizedEntry(), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->endFactorizedEntry(), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->setParentModality("foo", "plop"), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->setVariableValues(aSequence), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->endFactorizedProbabilityDeclaration(),
                      const gum::OperationNotAllowed&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(delete factory);
    }

    void testLegalsCallsInNONE() const {
      gum::BayesNetFactory< double >* factory = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d));

      CHECK_THROWS_AS(__bn_d->property("name"), const gum::NotFound&);
      CHECK_THROWS_AS(__bn_d->property("author"), const gum::NotFound&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startNetworkDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("name", "TestSuite BayesNet"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("author", "Lionel"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endNetworkDeclaration());

      CHECK((__bn_d->property("name")) == ("TestSuite BayesNet"));
      CHECK((__bn_d->property("author")) == ("Lionel"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(delete factory);
    }

    void testIllegalCallsInNETWORK() const {
      std::vector< float >            aSequence;
      gum::BayesNetFactory< double >* factory = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d));

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startNetworkDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("name", "TestSuite BayesNet"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("author", "Lionel"));

      CHECK_THROWS_AS(factory->variableName("foo"), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->variableDescription("bar"), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->variableType(gum::VarType::LABELIZED),
                      const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->addModality("plop"), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->setVariableCPTImplementation(0), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->endVariableDeclaration(), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->addParent("foo"), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->endParentsDeclaration(), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->rawConditionalTable(aSequence), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->endRawProbabilityDeclaration(), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->startFactorizedEntry(), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->endFactorizedEntry(), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->setParentModality("foo", "plop"), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->setVariableValues(aSequence), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(factory->endFactorizedProbabilityDeclaration(),
                      const gum::OperationNotAllowed&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endNetworkDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete factory);
    }

    void testBayesNetCreation() const {
      gum::BayesNetFactory< double >* factory = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d));

      // defining network
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startNetworkDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("name", "TestSuite BayesNet"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("author", "Lionel"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endNetworkDeclaration());

      // defining variables
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("Variable 1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("true"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("false"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("2"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 2"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("true"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("false"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("3"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 3"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("true"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("false"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("4"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 4"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("true"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("false"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("5"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 5"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("true"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("false"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      // Defining parents
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startParentsDeclaration("3"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addParent("1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endParentsDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startParentsDeclaration("4"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addParent("1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addParent("2"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endParentsDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startParentsDeclaration("5"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addParent("2"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addParent("3"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addParent("4"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endParentsDeclaration());

      // defining cpt
      std::vector< float > cpt_1;
      cpt_1.push_back(static_cast< float >(0.2));   // 1 : true
      cpt_1.push_back(static_cast< float >(0.8));   // 1 : false
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startRawProbabilityDeclaration("1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->rawConditionalTable(cpt_1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endRawProbabilityDeclaration());

      std::vector< float > cpt_2;
      cpt_2.push_back(static_cast< float >(0.2));   // 2 : true
      cpt_2.push_back(static_cast< float >(0.8));   // 2 : false
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startRawProbabilityDeclaration("2"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->rawConditionalTable(cpt_2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endRawProbabilityDeclaration());

      std::vector< float > cpt_3;
      cpt_3.push_back(static_cast< float >(0.2));   // 3 : true  given 1 : true
      cpt_3.push_back(static_cast< float >(0.5));   // 3 : true  given 1 : false
      cpt_3.push_back(static_cast< float >(0.8));   // 3 : false given 1 : true
      cpt_3.push_back(static_cast< float >(0.5));   // 3 : false given 1 : false
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startRawProbabilityDeclaration("3"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->rawConditionalTable(cpt_3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endRawProbabilityDeclaration());

      std::vector< float > cpt_4;
      cpt_4.push_back(static_cast< float >(0.2));     // 4 : true  given 1 : true,  2 : true
      cpt_4.push_back(static_cast< float >(0.5));     // 4 : true  given 1 : true,  2 : false
      cpt_4.push_back(static_cast< float >(0.65));    // 4 : true  given 1 : false, 2 : true
      cpt_4.push_back(static_cast< float >(0.001));   // 4 : true  given 1 : false, 2 : false
      cpt_4.push_back(static_cast< float >(0.8));     // 4 : false given 1 : true,  2 : true
      cpt_4.push_back(static_cast< float >(0.5));     // 4 : false given 1 : true,  2 : false
      cpt_4.push_back(static_cast< float >(0.35));    // 4 : false given 1 : false, 2 : true
      cpt_4.push_back(static_cast< float >(0.999));   // 4 : false given 1 : false, 2 : false
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startRawProbabilityDeclaration("4"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->rawConditionalTable(cpt_4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endRawProbabilityDeclaration());

      std::vector< float > values_5;
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startFactorizedProbabilityDeclaration("5"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startFactorizedEntry());
      values_5.push_back(static_cast< float >(0.5));   // 5 : true  given *
      values_5.push_back(static_cast< float >(0.5));   // 5 : false given *
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->setVariableValues(values_5));
      values_5.clear();
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endFactorizedEntry());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startFactorizedEntry());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->setParentModality("2", "true"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->setParentModality("4", "true"));
      values_5.push_back(static_cast< float >(1));   // 5 : true  given 2 : true, 4 : true
      values_5.push_back(static_cast< float >(0));   // 5 : false given 2 : true, 4 : true
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->setVariableValues(values_5));
      values_5.clear();
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endFactorizedEntry());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startFactorizedEntry());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->setParentModality("2", "false"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->setParentModality("3", "false"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->setParentModality("4", "true"));
      values_5.push_back(
          static_cast< float >(0));   // 5 : true  given 2 : false, 3 : false, 4 : true
      values_5.push_back(
          static_cast< float >(1));   // 5 : false given 2 : false, 3 : false, 4 : true
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->setVariableValues(values_5));
      values_5.clear();
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endFactorizedEntry());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endFactorizedProbabilityDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(delete factory);
    }

    void testDefiningWithAlltypes() const {
      gum::BayesNetFactory< double >* factory = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d));

      // defining network
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startNetworkDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("name", "TestSuite BayesNet"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("author", "Gaspard"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endNetworkDeclaration());

      // defining variable using variable type
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("Variable 1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("true"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("false"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      // defining variables without using variableType
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("2"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("Variable 2"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("true"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("false"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("3"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 3"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::RANGE));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addMin(0));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addMax(10));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("4"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 4"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::DISCRETIZED));
      for (int i = 10; i <= 50; i += 5) {
        GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addTick(i));
      }
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("5"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 5"));
      CHECK_THROWS_AS(factory->variableType(gum::VarType::CONTINUOUS),
                      const gum::OperationNotAllowed&);
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::INTEGER));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("7"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("3"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(delete factory);
    }

    void testDefiningWithCPT() const {
      gum::BayesNetFactory< double >* factory = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d));

      // defining network
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startNetworkDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("name", "TestSuite BayesNet"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("author", "Lionel"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endNetworkDeclaration());

      // defining variables
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("Variable 1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("true"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("false"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("2"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 2"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("true"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("false"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("3"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 3"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("true"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("false"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("4"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 4"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("true"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("false"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration());
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableName("5"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 5"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::INTEGER));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->addModality("0"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration());

      // Defining CPT
      auto                         cpt_1 = new gum::Tensor< double >();
      const gum::DiscreteVariable& var_1 = factory->bayesNet()->variable(factory->variableId("1"));
      cpt_1->add(var_1);
      gum::Instantiation inst_1(*cpt_1);
      inst_1.chgVal(var_1, 0);
      cpt_1->set(inst_1, 0.2);
      inst_1.chgVal(var_1, 1);
      cpt_1->set(inst_1, 0.8);
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->setVariableCPT("1", cpt_1, true));

      auto                         cpt_2 = new gum::Tensor< double >();
      const gum::DiscreteVariable& var_2 = factory->bayesNet()->variable(factory->variableId("2"));
      cpt_2->add(var_2);
      gum::Instantiation inst_2(*cpt_2);
      inst_2.chgVal(var_2, 0);
      cpt_2->set(inst_2, 0.2);
      inst_2.chgVal(var_2, 1);
      cpt_2->set(inst_2, 0.8);
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->setVariableCPT("2", cpt_2, true));

      auto                         cpt_3 = new gum::Tensor< double >();
      const gum::DiscreteVariable& var_3 = factory->bayesNet()->variable(factory->variableId("3"));
      cpt_3->add(var_3);
      cpt_3->add(var_1);
      gum::Instantiation inst_3(*cpt_3);
      inst_3.chgVal(var_3, 0);
      inst_3.chgVal(var_1, 0);
      cpt_3->set(inst_3, 0.2);
      inst_3.chgVal(var_3, 1);
      inst_3.chgVal(var_1, 0);
      cpt_3->set(inst_3, 0.8);
      inst_3.chgVal(var_3, 0);
      inst_3.chgVal(var_1, 1);
      cpt_3->set(inst_3, 0.5);
      inst_3.chgVal(var_3, 1);
      inst_3.chgVal(var_1, 1);
      cpt_3->set(inst_3, 0.5);
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->setVariableCPT("3", cpt_3, true));

      auto                         cpt_4 = new gum::Tensor< double >();
      const gum::DiscreteVariable& var_4 = factory->bayesNet()->variable(factory->variableId("4"));
      cpt_4->add(var_4);
      cpt_4->add(var_1);
      cpt_4->add(var_2);
      gum::Instantiation inst_4(*cpt_4);
      inst_4.chgVal(var_4, 0);
      inst_4.chgVal(var_1, 0);
      inst_4.chgVal(var_2, 0);
      cpt_4->set(inst_4, 0.2);
      inst_4.chgVal(var_4, 1);
      inst_4.chgVal(var_1, 0);
      inst_4.chgVal(var_2, 0);
      cpt_4->set(inst_4, 0.8);
      inst_4.chgVal(var_4, 0);
      inst_4.chgVal(var_1, 1);
      inst_4.chgVal(var_2, 0);
      cpt_4->set(inst_4, 0.65);
      inst_4.chgVal(var_4, 1);
      inst_4.chgVal(var_1, 1);
      inst_4.chgVal(var_2, 0);
      cpt_4->set(inst_4, 0.35);
      inst_4.chgVal(var_4, 0);
      inst_4.chgVal(var_1, 0);
      inst_4.chgVal(var_2, 1);
      cpt_4->set(inst_4, 0.5);
      inst_4.chgVal(var_4, 1);
      inst_4.chgVal(var_1, 0);
      inst_4.chgVal(var_2, 1);
      cpt_4->set(inst_4, 0.5);
      inst_4.chgVal(var_4, 0);
      inst_4.chgVal(var_1, 1);
      inst_4.chgVal(var_2, 1);
      cpt_4->set(inst_4, 0.001);
      inst_4.chgVal(var_4, 1);
      inst_4.chgVal(var_1, 1);
      inst_4.chgVal(var_2, 1);
      cpt_4->set(inst_4, 0.999);
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->setVariableCPT("4", cpt_4, true));

      auto mds   = new gum::MultiDimSparse< double >(0.5);
      auto cpt_5 = new gum::Tensor< double >(mds);

      const gum::DiscreteVariable& var_5 = factory->bayesNet()->variable(factory->variableId("5"));
      cpt_5->add(var_5);
      cpt_5->add(var_2);
      cpt_5->add(var_3);
      cpt_5->add(var_4);
      gum::Instantiation iter(*cpt_5);
      gum::Instantiation inst_5;
      inst_5 << var_5 << var_2 << var_4;
      inst_5.chgVal(var_5, 0);
      inst_5.chgVal(var_2, 0);
      inst_5.chgVal(var_4, 0);

      for (iter.setFirstOut(inst_5); !iter.end(); iter.incOut(inst_5)) {
        cpt_5->set(iter, static_cast< double >(1));
      }

      inst_5.chgVal(var_5, 1);

      for (iter.setFirstOut(inst_5); !iter.end(); iter.incOut(inst_5)) {
        cpt_5->set(iter, static_cast< double >(0));
      }

      iter.chgVal(var_5, 0);

      iter.chgVal(var_2, 1);
      iter.chgVal(var_3, 1);
      iter.chgVal(var_4, 0);
      cpt_5->set(iter, static_cast< double >(0));
      iter.chgVal(var_5, 1);
      cpt_5->set(iter, static_cast< double >(1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory->setVariableCPT("5", cpt_5, true));

      GUM_CHECK_ASSERT_THROWS_NOTHING(delete factory);
    }
  };

  GUM_TEST_ACTIF(Creation)
  GUM_TEST_ACTIF(IllegalCallsInNONE)
  GUM_TEST_ACTIF(LegalsCallsInNONE)
  GUM_TEST_ACTIF(IllegalCallsInNETWORK)
  GUM_TEST_ACTIF(BayesNetCreation)
  GUM_TEST_ACTIF(DefiningWithAlltypes)
  GUM_TEST_ACTIF(DefiningWithCPT)
}   // namespace gum_tests
