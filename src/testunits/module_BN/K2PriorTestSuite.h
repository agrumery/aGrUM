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

#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/BN/learning/priors/K2Prior.h>

namespace gum_tests {

  class [[maybe_unused]] K2PriorTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(1) {
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

        for (auto i = std::size_t(0); i < names.size(); ++i)
          trans_set.insertTranslator(i % 2 == 0 ? translator1 : translator2, i);
      }

      gum::learning::DatabaseTable database(trans_set);


      gum::learning::K2Prior prior(database);
      TS_ASSERT_EQUALS(prior.weight(), 1.0)
      prior.setWeight(4.0);
      TS_ASSERT_EQUALS(prior.weight(), 1.0)

      TS_ASSERT_EQUALS(prior.getType(), gum::learning::PriorType::SmoothingPriorType)

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
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(12, 1.0);
      prior.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }

      vect.clear();
      vect.resize(48, 1.0);
      prior.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 13.0)
      }


      gum::learning::K2Prior prior2(prior);
      TS_ASSERT_EQUALS(prior2.weight(), 1.0)
      prior2.setWeight(2.0);
      TS_ASSERT_EQUALS(prior2.weight(), 1.0)

      TS_ASSERT_EQUALS(prior2.getType(), gum::learning::PriorType::SmoothingPriorType)

      vect.clear();
      vect.resize(3, 1.0);
      prior2.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior2.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(12, 1.0);
      prior2.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior2.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior2.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      vect.resize(48, 1.0);
      prior2.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 13.0)
      }


      gum::learning::K2Prior prior3(std::move(prior2));
      TS_ASSERT_EQUALS(prior3.weight(), 1.0)
      prior3.setWeight(4.0);
      TS_ASSERT_EQUALS(prior3.weight(), 1.0)

      TS_ASSERT_EQUALS(prior3.getType(), gum::learning::PriorType::SmoothingPriorType)

      vect.clear();
      vect.resize(3, 1.0);
      prior3.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior3.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(12, 1.0);
      prior3.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior3.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior3.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      vect.resize(48, 1.0);
      prior3.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 13.0)
      }


      gum::learning::K2Prior* prior4 = prior.clone();
      TS_ASSERT_EQUALS(prior4->weight(), 1.0)
      prior4->setWeight(2.0);
      TS_ASSERT_EQUALS(prior4->weight(), 1.0)

      TS_ASSERT_EQUALS(prior4->getType(), gum::learning::PriorType::SmoothingPriorType)

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior4->addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(12, 1.0);
      prior4->addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior4->addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior4->addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      vect.resize(48, 1.0);
      prior4->addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 13.0)
      }

      delete prior4;


      gum::learning::DatabaseTable database2;
      gum::learning::K2Prior       prior5(database2);
      prior5 = prior;
      TS_ASSERT_EQUALS(prior5.weight(), 1.0)
      prior5.setWeight(2.0);
      TS_ASSERT_EQUALS(prior5.weight(), 1.0)

      TS_ASSERT_EQUALS(prior5.getType(), gum::learning::PriorType::SmoothingPriorType)

      vect.clear();
      vect.resize(3, 1.0);
      prior5.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(12, 1.0);
      prior5.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior5.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      vect.resize(48, 1.0);
      prior5.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 13.0)
      }

      prior5 = std::move(prior);
      TS_ASSERT_EQUALS(prior5.weight(), 1.0)
      prior5.setWeight(1.0);
      TS_ASSERT_EQUALS(prior5.weight(), 1.0)

      TS_ASSERT_EQUALS(prior5.getType(), gum::learning::PriorType::SmoothingPriorType)

      vect.clear();
      vect.resize(3, 1.0);
      prior5.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(12, 1.0);
      prior5.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior5.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      vect.resize(48, 1.0);
      prior5.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 13.0)
      }
    }

    GUM_ACTIVE_TEST(2) {
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

        for (auto i = std::size_t(0); i < names.size(); ++i)
          if (i % 2 == 0) {
            trans_set.insertTranslator(translator1, i);
          } else {
            trans_set.insertTranslator(translator2, i);
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


      gum::learning::K2Prior prior(database, nodeId2columns);
      TS_ASSERT_EQUALS(prior.weight(), 1.0)
      prior.setWeight(4.0);
      TS_ASSERT_EQUALS(prior.weight(), 1.0)

      TS_ASSERT_EQUALS(prior.getType(), gum::learning::PriorType::SmoothingPriorType)

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
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(16, 1.0);
      prior.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      vect.resize(36, 1.0);
      prior.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 17.0)
      }


      gum::learning::K2Prior prior2(prior);
      TS_ASSERT_EQUALS(prior2.weight(), 1.0)
      prior2.setWeight(2.0);
      TS_ASSERT_EQUALS(prior2.weight(), 1.0)

      TS_ASSERT_EQUALS(prior2.getType(), gum::learning::PriorType::SmoothingPriorType)

      vect.clear();
      vect.resize(4, 1.0);
      prior2.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior2.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(16, 1.0);
      prior2.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior2.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior2.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      vect.resize(36, 1.0);
      prior2.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 17.0)
      }

      gum::learning::K2Prior prior3(std::move(prior2));
      TS_ASSERT_EQUALS(prior3.weight(), 1.0)
      prior3.setWeight(4.0);
      TS_ASSERT_EQUALS(prior3.weight(), 1.0)

      TS_ASSERT_EQUALS(prior3.getType(), gum::learning::PriorType::SmoothingPriorType)

      vect.clear();
      vect.resize(4, 1.0);
      prior3.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior3.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(16, 1.0);
      prior3.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior3.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior3.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      vect.resize(36, 1.0);
      prior3.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 17.0)
      }


      gum::learning::K2Prior* prior4 = prior.clone();
      TS_ASSERT_EQUALS(prior4->weight(), 1.0)
      prior4->setWeight(2.0);
      TS_ASSERT_EQUALS(prior4->weight(), 1.0)

      TS_ASSERT_EQUALS(prior4->getType(), gum::learning::PriorType::SmoothingPriorType)

      vect.clear();
      vect.resize(4, 1.0);
      prior4->addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior4->addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(16, 1.0);
      prior4->addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior4->addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior4->addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      vect.resize(36, 1.0);
      prior4->addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 17.0)
      }

      delete prior4;

      gum::learning::DatabaseTable database2;
      gum::learning::K2Prior       prior5(database2);
      prior5 = prior;
      TS_ASSERT_EQUALS(prior5.weight(), 1.0)
      prior5.setWeight(2.0);
      TS_ASSERT_EQUALS(prior5.weight(), 1.0)

      TS_ASSERT_EQUALS(prior5.getType(), gum::learning::PriorType::SmoothingPriorType)

      vect.clear();
      vect.resize(4, 1.0);
      prior5.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(16, 1.0);
      prior5.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior5.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      vect.resize(36, 1.0);
      prior5.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 17.0)
      }


      prior5 = std::move(prior);
      TS_ASSERT_EQUALS(prior5.weight(), 1.0)
      prior5.setWeight(1.0);
      TS_ASSERT_EQUALS(prior5.weight(), 1.0)

      TS_ASSERT_EQUALS(prior5.getType(), gum::learning::PriorType::SmoothingPriorType)

      vect.clear();
      vect.resize(4, 1.0);
      prior5.addJointPseudoCount(idset1, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(16, 1.0);
      prior5.addJointPseudoCount(idset2, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      prior5.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(576, 1.0);
      prior5.addJointPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 2.0)
      }
      vect.clear();
      vect.resize(36, 1.0);
      prior5.addConditioningPseudoCount(idset3, vect);
      for (const auto val: vect) {
        TS_ASSERT_EQUALS(val, 17.0)
      }
    }
  };

} /* namespace gum_tests */
