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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/BN/learning/priors/bdeuPrior.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  BDeuPrior
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct BDeuPriorTestSuite {
    public:
    static void test1() {   // create the translator set
      gum::LabelizedVariable var1("X1", "", 0);
      var1.addLabel("0");
      var1.addLabel("1");
      var1.addLabel("2");

      gum::LabelizedVariable var2("X2", "", 0);
      var2.addLabel("0");
      var2.addLabel("1");
      var2.addLabel("2");
      var2.addLabel("3");

      gum::learning::DBTranslatorSet trans_set;
      {
        const std::vector< std::string >              miss;
        gum::learning::DBTranslator4LabelizedVariable translator1(var1, miss);
        gum::learning::DBTranslator4LabelizedVariable translator2(var2, miss);
        std::vector< std::string >                    names{"A", "B", "C", "D", "E", "F"};

        for (auto i = std::size_t(0); i < names.size(); ++i)
          trans_set.insertTranslator(i % 2 == 0 ? translator1 : translator2, i);
      }

      gum::learning::DatabaseTable database(trans_set);


      gum::learning::BDeuPrior prior(database);
      CHECK((prior.weight()) == (1.0));
      prior.setWeight(4.0);
      CHECK((prior.weight()) == (4.0));

      CHECK((prior.getType()) == (gum::learning::PriorType::BDeuPriorType));

      gum::NodeId                node0 = 0;
      gum::NodeId                node1 = 1;
      gum::NodeId                node3 = 3;
      gum::NodeId                node4 = 4;
      gum::NodeId                node5 = 5;
      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond1{node3, node5, node4};

      gum::learning::IdCondSet idset1(node0, cond_empty);   // #3,#0
      gum::learning::IdCondSet idset2(node0, node1, cond_empty,
                                      true);                // #12,#0
      gum::learning::IdCondSet idset3(node1, node0, cond1, true,
                                      true);                // #576,#48

      std::vector< double > vect(3, 1.0);
      prior.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 3)));
      }
      vect.clear();
      prior.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(12, 1.0);
      prior.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 12)));
      }
      vect.clear();
      prior.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 576)));
      }

      vect.clear();
      vect.resize(48, 1.0);
      prior.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 48)));
      }


      gum::learning::BDeuPrior prior2(prior);
      CHECK((prior2.weight()) == (4.0));
      prior2.setWeight(2.0);
      CHECK((prior2.weight()) == (2.0));

      CHECK((prior2.getType()) == (gum::learning::PriorType::BDeuPriorType));

      vect.clear();
      vect.resize(3, 1.0);
      prior2.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 3)));
      }
      vect.clear();
      prior2.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(12, 1.0);
      prior2.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 12)));
      }
      vect.clear();
      prior2.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior2.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 576)));
      }
      vect.clear();
      vect.resize(48, 1.0);
      prior2.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 48)));
      }


      gum::learning::BDeuPrior prior3(std::move(prior2));
      CHECK((prior3.weight()) == (2.0));
      prior3.setWeight(4.0);
      CHECK((prior3.weight()) == (4.0));

      CHECK((prior3.getType()) == (gum::learning::PriorType::BDeuPriorType));

      vect.clear();
      vect.resize(3, 1.0);
      prior3.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 3)));
      }
      vect.clear();
      prior3.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(12, 1.0);
      prior3.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 12)));
      }
      vect.clear();
      prior3.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior3.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 576)));
      }
      vect.clear();
      vect.resize(48, 1.0);
      prior3.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 48)));
      }


      gum::learning::BDeuPrior* prior4 = prior.clone();
      CHECK((prior4->weight()) == (4.0));
      prior4->setWeight(2.0);
      CHECK((prior4->weight()) == (2.0));

      CHECK((prior4->getType()) == (gum::learning::PriorType::BDeuPriorType));

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 3)));
      }
      vect.clear();
      prior4->addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(12, 1.0);
      prior4->addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 12)));
      }
      vect.clear();
      prior4->addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior4->addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 576)));
      }
      vect.clear();
      vect.resize(48, 1.0);
      prior4->addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 48)));
      }

      delete prior4;


      gum::learning::DatabaseTable database2;
      gum::learning::BDeuPrior     prior5(database2);
      prior5 = prior;
      CHECK((prior5.weight()) == (4.0));
      prior5.setWeight(2.0);
      CHECK((prior5.weight()) == (2.0));

      CHECK((prior5.getType()) == (gum::learning::PriorType::BDeuPriorType));

      vect.clear();
      vect.resize(3, 1.0);
      prior5.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 3)));
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(12, 1.0);
      prior5.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 12)));
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior5.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 576)));
      }
      vect.clear();
      vect.resize(48, 1.0);
      prior5.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 48)));
      }

      prior5 = std::move(prior);
      CHECK((prior5.weight()) == (4.0));
      prior5.setWeight(1.0);
      CHECK((prior5.weight()) == (1.0));

      CHECK((prior5.getType()) == (gum::learning::PriorType::BDeuPriorType));

      vect.clear();
      vect.resize(3, 1.0);
      prior5.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 1.0 / 3)));
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(12, 1.0);
      prior5.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 1.0 / 12)));
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior5.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 1.0 / 576)));
      }
      vect.clear();
      vect.resize(48, 1.0);
      prior5.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 1.0 / 48)));
      }
    }   // namespace gum_tests

    static void test2() {
      // create the translator set
      gum::LabelizedVariable var1("X1", "", 0);
      var1.addLabel("0");
      var1.addLabel("1");
      var1.addLabel("2");

      gum::LabelizedVariable var2("X2", "", 0);
      var2.addLabel("0");
      var2.addLabel("1");
      var2.addLabel("2");
      var2.addLabel("3");

      gum::learning::DBTranslatorSet trans_set;
      {
        const std::vector< std::string >              miss;
        gum::learning::DBTranslator4LabelizedVariable translator1(var1, miss);
        gum::learning::DBTranslator4LabelizedVariable translator2(var2, miss);
        std::vector< std::string >                    names{"A", "B", "C", "D", "E", "F"};

        for (auto i = std::size_t(0); i < names.size(); ++i) {
          if (i % 2 == 0) {
            trans_set.insertTranslator(translator1, i);
          } else {
            trans_set.insertTranslator(translator2, i);
          }
        }
      }

      gum::learning::DatabaseTable database(trans_set);

      gum::NodeId                                node0 = 0;
      gum::NodeId                                node1 = 1;
      gum::NodeId                                node2 = 2;
      gum::NodeId                                node3 = 3;
      gum::NodeId                                node4 = 4;
      gum::NodeId                                node5 = 5;
      gum::Bijection< gum::NodeId, std::size_t > nodeId2columns;
      nodeId2columns.insert(node0, std::size_t(1));
      nodeId2columns.insert(node1, std::size_t(3));
      nodeId2columns.insert(node2, std::size_t(0));
      nodeId2columns.insert(node3, std::size_t(2));
      nodeId2columns.insert(node4, std::size_t(4));
      nodeId2columns.insert(node5, std::size_t(5));


      gum::learning::BDeuPrior prior(database, nodeId2columns);
      CHECK((prior.weight()) == (1.0));
      prior.setWeight(4.0);
      CHECK((prior.weight()) == (4.0));

      CHECK((prior.getType()) == (gum::learning::PriorType::BDeuPriorType));

      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond1{node3, node5, node4};

      gum::learning::IdCondSet idset1(node0, cond_empty);   // #4,#0
      gum::learning::IdCondSet idset2(node0, node1, cond_empty,
                                      true);                // #16,#0
      gum::learning::IdCondSet idset3(node1, node0, cond1, true,
                                      true);                // #576,#36

      std::vector< double > vect(4, 1.0);
      prior.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 4)));
      }
      vect.clear();
      prior.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(16, 1.0);
      prior.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 16)));
      }
      vect.clear();
      prior.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 576)));
      }
      vect.clear();
      vect.resize(36, 1.0);
      prior.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 36)));
      }


      gum::learning::BDeuPrior prior2(prior);
      CHECK((prior2.weight()) == (4.0));
      prior2.setWeight(2.0);
      CHECK((prior2.weight()) == (2.0));

      CHECK((prior2.getType()) == (gum::learning::PriorType::BDeuPriorType));

      vect.clear();
      vect.resize(4, 1.0);
      prior2.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 4)));
      }
      vect.clear();
      prior2.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(16, 1.0);
      prior2.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 16)));
      }
      vect.clear();
      prior2.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior2.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 576)));
      }
      vect.clear();
      vect.resize(36, 1.0);
      prior2.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 36)));
      }

      gum::learning::BDeuPrior prior3(std::move(prior2));
      CHECK((prior3.weight()) == (2.0));
      prior3.setWeight(4.0);
      CHECK((prior3.weight()) == (4.0));

      CHECK((prior3.getType()) == (gum::learning::PriorType::BDeuPriorType));

      vect.clear();
      vect.resize(4, 1.0);
      prior3.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 4)));
      }
      vect.clear();
      prior3.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(16, 1.0);
      prior3.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 16)));
      }
      vect.clear();
      prior3.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior3.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 576)));
      }
      vect.clear();
      vect.resize(36, 1.0);
      prior3.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 4.0 / 36)));
      }


      gum::learning::BDeuPrior* prior4 = prior.clone();
      CHECK((prior4->weight()) == (4.0));
      prior4->setWeight(2.0);
      CHECK((prior4->weight()) == (2.0));

      CHECK((prior4->getType()) == (gum::learning::PriorType::BDeuPriorType));

      vect.clear();
      vect.resize(4, 1.0);
      prior4->addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 4)));
      }
      vect.clear();
      prior4->addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(16, 1.0);
      prior4->addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 16)));
      }
      vect.clear();
      prior4->addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior4->addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 576)));
      }
      vect.clear();
      vect.resize(36, 1.0);
      prior4->addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 36)));
      }

      delete prior4;

      gum::learning::DatabaseTable database2;
      gum::learning::BDeuPrior     prior5(database2);
      prior5 = prior;
      CHECK((prior5.weight()) == (4.0));
      prior5.setWeight(2.0);
      CHECK((prior5.weight()) == (2.0));

      CHECK((prior5.getType()) == (gum::learning::PriorType::BDeuPriorType));

      vect.clear();
      vect.resize(4, 1.0);
      prior5.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 4)));
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(16, 1.0);
      prior5.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 16)));
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior5.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 576)));
      }
      vect.clear();
      vect.resize(36, 1.0);
      prior5.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 2.0 / 36)));
      }


      prior5 = std::move(prior);
      CHECK((prior5.weight()) == (4.0));
      prior5.setWeight(1.0);
      CHECK((prior5.weight()) == (1.0));

      CHECK((prior5.getType()) == (gum::learning::PriorType::BDeuPriorType));

      vect.clear();
      vect.resize(4, 1.0);
      prior5.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 1.0 / 4)));
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(16, 1.0);
      prior5.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 1.0 / 16)));
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior5.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 1.0 / 576)));
      }
      vect.clear();
      vect.resize(36, 1.0);
      prior5.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        CHECK((val) == ((1.0 + 1.0 / 36)));
      }
    }
  };

  GUM_TEST_ACTIF(1)
  GUM_TEST_ACTIF(2)

} /* namespace gum_tests */
