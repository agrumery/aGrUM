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
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/multidim/implementations/multiDimSparse.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/BayesNetFactory.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class [[maybe_unused]] BayesNetFactoryTestSuite: public CxxTest::TestSuite {
    private:
    gum::BayesNet< double >* __bn_d;
    // gum::BayesNet<float>*   __bn_f;

    public:
    void setUp() final { __bn_d = new gum::BayesNet< double >(); }

    void tearDown() final { delete __bn_d; }

    GUM_ACTIVE_TEST(Creation) {
      gum::BayesNetFactory< double >* factory = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d))
      TS_GUM_ASSERT_THROWS_NOTHING(delete factory)
    }

    GUM_ACTIVE_TEST(IllegalCallsInNONE) {
      std::vector< float >            aSequence;
      gum::BayesNetFactory< double >* factory = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d))

      TS_ASSERT_THROWS(factory->variableName("foo"), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->variableDescription("bar"), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->variableType(gum::VarType::LABELIZED),
                       const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->addModality("plop"), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->setVariableCPTImplementation(0), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->endVariableDeclaration(), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->addParent("foo"), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->endParentsDeclaration(), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->rawConditionalTable(aSequence), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->endRawProbabilityDeclaration(), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->startFactorizedEntry(), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->endFactorizedEntry(), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->setParentModality("foo", "plop"), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->setVariableValues(aSequence), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->endFactorizedProbabilityDeclaration(),
                       const gum::OperationNotAllowed&)

      TS_GUM_ASSERT_THROWS_NOTHING(delete factory)
    }

    GUM_ACTIVE_TEST(LegalsCallsInNONE) {
      gum::BayesNetFactory< double >* factory = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d))

      TS_ASSERT_THROWS(__bn_d->property("name"), const gum::NotFound&)
      TS_ASSERT_THROWS(__bn_d->property("author"), const gum::NotFound&)

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startNetworkDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("name", "TestSuite BayesNet"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("author", "Lionel"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endNetworkDeclaration())

      TS_ASSERT_EQUALS(__bn_d->property("name"), "TestSuite BayesNet")
      TS_ASSERT_EQUALS(__bn_d->property("author"), "Lionel")

      TS_GUM_ASSERT_THROWS_NOTHING(delete factory)
    }

    GUM_ACTIVE_TEST(IllegalCallsInNETWORK) {
      std::vector< float >            aSequence;
      gum::BayesNetFactory< double >* factory = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d))

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startNetworkDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("name", "TestSuite BayesNet"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("author", "Lionel"))

      TS_ASSERT_THROWS(factory->variableName("foo"), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->variableDescription("bar"), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->variableType(gum::VarType::LABELIZED),
                       const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->addModality("plop"), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->setVariableCPTImplementation(0), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->endVariableDeclaration(), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->addParent("foo"), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->endParentsDeclaration(), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->rawConditionalTable(aSequence), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->endRawProbabilityDeclaration(), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->startFactorizedEntry(), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->endFactorizedEntry(), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->setParentModality("foo", "plop"), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->setVariableValues(aSequence), const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(factory->endFactorizedProbabilityDeclaration(),
                       const gum::OperationNotAllowed&)

      TS_GUM_ASSERT_THROWS_NOTHING(factory->endNetworkDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(delete factory)
    }

    GUM_ACTIVE_TEST(BayesNetCreation) {
      gum::BayesNetFactory< double >* factory = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d))

      // defining network
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startNetworkDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("name", "TestSuite BayesNet"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("author", "Lionel"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endNetworkDeclaration())

      // defining variables
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("1"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("Variable 1"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("true"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("false"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("2"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 2"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("true"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("false"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("3"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 3"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("true"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("false"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("4"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 4"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("true"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("false"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("5"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 5"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("true"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("false"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      // Defining parents
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startParentsDeclaration("3"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addParent("1"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endParentsDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startParentsDeclaration("4"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addParent("1"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addParent("2"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endParentsDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startParentsDeclaration("5"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addParent("2"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addParent("3"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addParent("4"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endParentsDeclaration())

      // defining cpt
      std::vector< float > cpt_1;
      cpt_1.push_back((float)0.2);   // 1 : true
      cpt_1.push_back((float)0.8);   // 1 : false
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startRawProbabilityDeclaration("1"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->rawConditionalTable(cpt_1))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endRawProbabilityDeclaration())

      std::vector< float > cpt_2;
      cpt_2.push_back((float)0.2);   // 2 : true
      cpt_2.push_back((float)0.8);   // 2 : false
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startRawProbabilityDeclaration("2"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->rawConditionalTable(cpt_2))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endRawProbabilityDeclaration())

      std::vector< float > cpt_3;
      cpt_3.push_back((float)0.2);   // 3 : true  given 1 : true
      cpt_3.push_back((float)0.5);   // 3 : true  given 1 : false
      cpt_3.push_back((float)0.8);   // 3 : false given 1 : true
      cpt_3.push_back((float)0.5);   // 3 : false given 1 : false
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startRawProbabilityDeclaration("3"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->rawConditionalTable(cpt_3))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endRawProbabilityDeclaration())

      std::vector< float > cpt_4;
      cpt_4.push_back((float)0.2);     // 4 : true  given 1 : true,  2 : true
      cpt_4.push_back((float)0.5);     // 4 : true  given 1 : true,  2 : false
      cpt_4.push_back((float)0.65);    // 4 : true  given 1 : false, 2 : true
      cpt_4.push_back((float)0.001);   // 4 : true  given 1 : false, 2 : false
      cpt_4.push_back((float)0.8);     // 4 : false given 1 : true,  2 : true
      cpt_4.push_back((float)0.5);     // 4 : false given 1 : true,  2 : false
      cpt_4.push_back((float)0.35);    // 4 : false given 1 : false, 2 : true
      cpt_4.push_back((float)0.999);   // 4 : false given 1 : false, 2 : false
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startRawProbabilityDeclaration("4"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->rawConditionalTable(cpt_4))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endRawProbabilityDeclaration())

      std::vector< float > values_5;
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startFactorizedProbabilityDeclaration("5"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startFactorizedEntry())
      values_5.push_back((float)0.5);   // 5 : true  given *
      values_5.push_back((float)0.5);   // 5 : false given *
      TS_GUM_ASSERT_THROWS_NOTHING(factory->setVariableValues(values_5))
      values_5.clear();
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endFactorizedEntry())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startFactorizedEntry())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->setParentModality("2", "true"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->setParentModality("4", "true"))
      values_5.push_back((float)1);   // 5 : true  given 2 : true, 4 : true
      values_5.push_back((float)0);   // 5 : false given 2 : true, 4 : true
      TS_GUM_ASSERT_THROWS_NOTHING(factory->setVariableValues(values_5))
      values_5.clear();
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endFactorizedEntry())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startFactorizedEntry())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->setParentModality("2", "false"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->setParentModality("3", "false"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->setParentModality("4", "true"))
      values_5.push_back((float)0);   // 5 : true  given 2 : false, 3 : false, 4 : true
      values_5.push_back((float)1);   // 5 : false given 2 : false, 3 : false, 4 : true
      TS_GUM_ASSERT_THROWS_NOTHING(factory->setVariableValues(values_5))
      values_5.clear();
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endFactorizedEntry())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endFactorizedProbabilityDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(delete factory)
    }

    GUM_ACTIVE_TEST(DefiningWithAlltypes) {
      gum::BayesNetFactory< double >* factory = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d))

      // defining network
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startNetworkDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("name", "TestSuite BayesNet"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("author", "Gaspard"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endNetworkDeclaration())

      // defining variable using variable type
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("1"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("Variable 1"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("true"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("false"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      // defining variables without using variableType
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("2"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("Variable 2"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("true"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("false"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("3"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 3"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::RANGE))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addMin(0))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addMax(10))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("4"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 4"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::DISCRETIZED))
      for (int i = 10; i <= 50; i += 5) {
        TS_GUM_ASSERT_THROWS_NOTHING(factory->addTick(i))
      }
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("5"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 5"))
      TS_ASSERT_THROWS(factory->variableType(gum::VarType::CONTINUOUS),
                       const gum::OperationNotAllowed&)
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::INTEGER))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("1"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("7"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("3"))

      TS_GUM_ASSERT_THROWS_NOTHING(delete factory)
    }

    GUM_ACTIVE_TEST(DefiningWithCPT) {
      gum::BayesNetFactory< double >* factory = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(factory = new gum::BayesNetFactory< double >(__bn_d))

      // defining network
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startNetworkDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("name", "TestSuite BayesNet"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addNetworkProperty("author", "Lionel"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endNetworkDeclaration())

      // defining variables
      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("1"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("Variable 1"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("true"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("false"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("2"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 2"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("true"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("false"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("3"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 3"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("true"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("false"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("4"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 4"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::LABELIZED))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("true"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("false"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      TS_GUM_ASSERT_THROWS_NOTHING(factory->startVariableDeclaration())
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableName("5"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableDescription("variable 5"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->variableType(gum::VarType::INTEGER))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("1"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->addModality("0"))
      TS_GUM_ASSERT_THROWS_NOTHING(factory->endVariableDeclaration())

      // Defining CPT
      auto                         cpt_1 = new gum::Tensor< double >();
      const gum::DiscreteVariable& var_1 = factory->bayesNet()->variable(factory->variableId("1"));
      cpt_1->add(var_1);
      gum::Instantiation inst_1(*cpt_1);
      inst_1.chgVal(var_1, 0);
      cpt_1->set(inst_1, 0.2);
      inst_1.chgVal(var_1, 1);
      cpt_1->set(inst_1, 0.8);
      TS_GUM_ASSERT_THROWS_NOTHING(factory->setVariableCPT("1", cpt_1, true))

      auto                         cpt_2 = new gum::Tensor< double >();
      const gum::DiscreteVariable& var_2 = factory->bayesNet()->variable(factory->variableId("2"));
      cpt_2->add(var_2);
      gum::Instantiation inst_2(*cpt_2);
      inst_2.chgVal(var_2, 0);
      cpt_2->set(inst_2, 0.2);
      inst_2.chgVal(var_2, 1);
      cpt_2->set(inst_2, 0.8);
      TS_GUM_ASSERT_THROWS_NOTHING(factory->setVariableCPT("2", cpt_2, true))

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
      TS_GUM_ASSERT_THROWS_NOTHING(factory->setVariableCPT("3", cpt_3, true))

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
      TS_GUM_ASSERT_THROWS_NOTHING(factory->setVariableCPT("4", cpt_4, true))

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
        cpt_5->set(iter, (double)1);
      }

      inst_5.chgVal(var_5, 1);

      for (iter.setFirstOut(inst_5); !iter.end(); iter.incOut(inst_5)) {
        cpt_5->set(iter, (double)0);
      }

      iter.chgVal(var_5, 0);

      iter.chgVal(var_2, 1);
      iter.chgVal(var_3, 1);
      iter.chgVal(var_4, 0);
      cpt_5->set(iter, (double)0);
      iter.chgVal(var_5, 1);
      cpt_5->set(iter, (double)1);
      TS_GUM_ASSERT_THROWS_NOTHING(factory->setVariableCPT("5", cpt_5, true))

      TS_GUM_ASSERT_THROWS_NOTHING(delete factory)
    }
  };
}   // namespace gum_tests
