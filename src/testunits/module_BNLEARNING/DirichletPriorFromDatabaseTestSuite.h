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
#include <agrum/BN/learning/priors/DirichletPriorFromDatabase.h>

namespace gum_tests {

  class [[maybe_unused]] DirichletPriorFromDatabaseTestSuite: public CxxTest::TestSuite {
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

        for (auto i = std::size_t(0); i < names.size(); ++i) {
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
      const auto db_size = (double)database.nbRows();

      // create the parser
      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);


      gum::learning::DirichletPriorFromDatabase prior(database, parser);

      TS_ASSERT_EQUALS(prior.weight(), 1.0)
      prior.setWeight(2.0 * db_size);
      TS_ASSERT_EQUALS(prior.weight(), 2.0 * db_size)

      TS_ASSERT_EQUALS(prior.getType(), gum::learning::PriorType::DirichletPriorType)

      gum::NodeId                node0 = 0;
      gum::NodeId                node1 = 1;
      gum::NodeId                node3 = 3;
      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond1{node3};

      gum::learning::IdCondSet idset1(node0, cond_empty);                // #3,#0
      gum::learning::IdCondSet idset2(node0, node1, cond_empty, true);   // #9,#0
      gum::learning::IdCondSet idset3(node0, cond1, true);               // #9,#0


      std::vector< double > vect(3, 1.0);
      prior.addJointPseudoCount(idset1, vect);
      TS_ASSERT_EQUALS(vect[0], 2401.0)
      TS_ASSERT_EQUALS(vect[1], 251.0)
      TS_ASSERT_EQUALS(vect[2], 151.0)

      vect.clear();
      prior.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior.addJointPseudoCount(idset2, vect);
      TS_ASSERT_EQUALS(vect[0], 401.0)    // 0,0
      TS_ASSERT_EQUALS(vect[1], 151.0)    // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 2001.0)   // 0,1
      TS_ASSERT_EQUALS(vect[4], 1.0)      // 1,1
      TS_ASSERT_EQUALS(vect[5], 151.0)    // 2,1
      TS_ASSERT_EQUALS(vect[6], 1.0)      // 0,2
      TS_ASSERT_EQUALS(vect[7], 101.0)    // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      prior.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior.addJointPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 1.0)      // 0,0
      TS_ASSERT_EQUALS(vect[1], 151.0)    // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 401.0)    // 0,1
      TS_ASSERT_EQUALS(vect[4], 101.0)    // 1,1
      TS_ASSERT_EQUALS(vect[5], 151.0)    // 2,1
      TS_ASSERT_EQUALS(vect[6], 2001.0)   // 0,2
      TS_ASSERT_EQUALS(vect[7], 1.0)      // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      prior.addConditioningPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 151.0)
      TS_ASSERT_EQUALS(vect[1], 651.0)
      TS_ASSERT_EQUALS(vect[2], 2001.0)


      gum::learning::DirichletPriorFromDatabase prior2(prior);
      TS_ASSERT_EQUALS(prior2.weight(), 2.0 * db_size)
      prior2.setWeight(1.0 * db_size);
      TS_ASSERT_EQUALS(prior2.weight(), db_size)

      TS_ASSERT_EQUALS(prior2.getType(), gum::learning::PriorType::DirichletPriorType)

      vect.clear();
      vect.resize(3, 1.0);
      prior2.addJointPseudoCount(idset1, vect);
      TS_ASSERT_EQUALS(vect[0], 1201.0)
      TS_ASSERT_EQUALS(vect[1], 126.0)
      TS_ASSERT_EQUALS(vect[2], 76.0)

      vect.clear();
      prior2.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior2.addJointPseudoCount(idset2, vect);
      TS_ASSERT_EQUALS(vect[0], 201.0)    // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 1001.0)   // 0,1
      TS_ASSERT_EQUALS(vect[4], 1.0)      // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1.0)      // 0,2
      TS_ASSERT_EQUALS(vect[7], 51.0)     // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      prior2.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior2.addJointPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 1.0)      // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 201.0)    // 0,1
      TS_ASSERT_EQUALS(vect[4], 51.0)     // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1001.0)   // 0,2
      TS_ASSERT_EQUALS(vect[7], 1.0)      // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      prior2.addConditioningPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 76.0)
      TS_ASSERT_EQUALS(vect[1], 326.0)
      TS_ASSERT_EQUALS(vect[2], 1001.0)


      gum::learning::DirichletPriorFromDatabase prior3(std::move(prior2));
      TS_ASSERT_EQUALS(prior3.weight(), db_size)
      prior3.setWeight(2.0 * db_size);
      TS_ASSERT_EQUALS(prior3.weight(), 2.0 * db_size)

      vect.clear();
      vect.resize(3, 1.0);
      prior3.addJointPseudoCount(idset1, vect);
      TS_ASSERT_EQUALS(vect[0], 2401.0)
      TS_ASSERT_EQUALS(vect[1], 251.0)
      TS_ASSERT_EQUALS(vect[2], 151.0)

      vect.clear();
      prior3.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior3.addJointPseudoCount(idset2, vect);
      TS_ASSERT_EQUALS(vect[0], 401.0)    // 0,0
      TS_ASSERT_EQUALS(vect[1], 151.0)    // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 2001.0)   // 0,1
      TS_ASSERT_EQUALS(vect[4], 1.0)      // 1,1
      TS_ASSERT_EQUALS(vect[5], 151.0)    // 2,1
      TS_ASSERT_EQUALS(vect[6], 1.0)      // 0,2
      TS_ASSERT_EQUALS(vect[7], 101.0)    // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      prior3.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior3.addJointPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 1.0)      // 0,0
      TS_ASSERT_EQUALS(vect[1], 151.0)    // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 401.0)    // 0,1
      TS_ASSERT_EQUALS(vect[4], 101.0)    // 1,1
      TS_ASSERT_EQUALS(vect[5], 151.0)    // 2,1
      TS_ASSERT_EQUALS(vect[6], 2001.0)   // 0,2
      TS_ASSERT_EQUALS(vect[7], 1.0)      // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      prior3.addConditioningPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 151.0)
      TS_ASSERT_EQUALS(vect[1], 651.0)
      TS_ASSERT_EQUALS(vect[2], 2001.0)


      gum::learning::DirichletPriorFromDatabase* prior4 = prior3.clone();
      TS_ASSERT_EQUALS(prior4->weight(), 2.0 * db_size)
      prior4->setWeight(1.0 * db_size);
      TS_ASSERT_EQUALS(prior4->weight(), 1.0 * db_size)

      TS_ASSERT_EQUALS(prior4->getType(), gum::learning::PriorType::DirichletPriorType)

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addJointPseudoCount(idset1, vect);
      TS_ASSERT_EQUALS(vect[0], 1201.0)
      TS_ASSERT_EQUALS(vect[1], 126.0)
      TS_ASSERT_EQUALS(vect[2], 76.0)

      vect.clear();
      prior4->addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior4->addJointPseudoCount(idset2, vect);
      TS_ASSERT_EQUALS(vect[0], 201.0)    // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 1001.0)   // 0,1
      TS_ASSERT_EQUALS(vect[4], 1.0)      // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1.0)      // 0,2
      TS_ASSERT_EQUALS(vect[7], 51.0)     // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      prior4->addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior4->addJointPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 1.0)      // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 201.0)    // 0,1
      TS_ASSERT_EQUALS(vect[4], 51.0)     // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1001.0)   // 0,2
      TS_ASSERT_EQUALS(vect[7], 1.0)      // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addConditioningPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 76.0)
      TS_ASSERT_EQUALS(vect[1], 326.0)
      TS_ASSERT_EQUALS(vect[2], 1001.0)


      prior4->operator=(prior);
      TS_ASSERT_EQUALS(prior4->weight(), 2.0 * db_size)
      prior4->setWeight(1.0 * db_size);
      TS_ASSERT_EQUALS(prior4->weight(), 1.0 * db_size)

      TS_ASSERT_EQUALS(prior4->getType(), gum::learning::PriorType::DirichletPriorType)

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addJointPseudoCount(idset1, vect);
      TS_ASSERT_EQUALS(vect[0], 1201.0)
      TS_ASSERT_EQUALS(vect[1], 126.0)
      TS_ASSERT_EQUALS(vect[2], 76.0)

      vect.clear();
      prior4->addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior4->addJointPseudoCount(idset2, vect);
      TS_ASSERT_EQUALS(vect[0], 201.0)    // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 1001.0)   // 0,1
      TS_ASSERT_EQUALS(vect[4], 1.0)      // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1.0)      // 0,2
      TS_ASSERT_EQUALS(vect[7], 51.0)     // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      prior4->addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior4->addJointPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 1.0)      // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 201.0)    // 0,1
      TS_ASSERT_EQUALS(vect[4], 51.0)     // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1001.0)   // 0,2
      TS_ASSERT_EQUALS(vect[7], 1.0)      // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addConditioningPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 76.0)
      TS_ASSERT_EQUALS(vect[1], 326.0)
      TS_ASSERT_EQUALS(vect[2], 1001.0)


      prior4->operator=(std::move(prior));
      TS_ASSERT_EQUALS(prior4->weight(), 2.0 * db_size)
      prior4->setWeight(1.0 * db_size);
      TS_ASSERT_EQUALS(prior4->weight(), 1.0 * db_size)

      TS_ASSERT_EQUALS(prior4->getType(), gum::learning::PriorType::DirichletPriorType)

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addJointPseudoCount(idset1, vect);
      TS_ASSERT_EQUALS(vect[0], 1201.0)
      TS_ASSERT_EQUALS(vect[1], 126.0)
      TS_ASSERT_EQUALS(vect[2], 76.0)

      vect.clear();
      prior4->addConditioningPseudoCount(idset1, vect);


      vect.clear();
      vect.resize(9, 1.0);
      prior4->addJointPseudoCount(idset2, vect);
      TS_ASSERT_EQUALS(vect[0], 201.0)    // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 1001.0)   // 0,1
      TS_ASSERT_EQUALS(vect[4], 1.0)      // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1.0)      // 0,2
      TS_ASSERT_EQUALS(vect[7], 51.0)     // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      prior4->addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior4->addJointPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 1.0)      // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 201.0)    // 0,1
      TS_ASSERT_EQUALS(vect[4], 51.0)     // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1001.0)   // 0,2
      TS_ASSERT_EQUALS(vect[7], 1.0)      // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addConditioningPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 76.0)
      TS_ASSERT_EQUALS(vect[1], 326.0)
      TS_ASSERT_EQUALS(vect[2], 1001.0)

      delete prior4;
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

        for (auto i = std::size_t(0); i < names.size(); ++i) {
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

      const std::size_t db_size = database.nbRows();

      // create the parser
      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);

      gum::Bijection< gum::NodeId, std::size_t > nodeId2columns;
      gum::NodeId                                node0 = 0;
      gum::NodeId                                node1 = 1;
      gum::NodeId                                node2 = 2;
      gum::NodeId                                node3 = 3;
      gum::NodeId                                node4 = 4;
      gum::NodeId                                node5 = 5;
      nodeId2columns.insert(node0, std::size_t(4));
      nodeId2columns.insert(node1, std::size_t(3));
      nodeId2columns.insert(node2, std::size_t(0));
      nodeId2columns.insert(node3, std::size_t(2));
      nodeId2columns.insert(node4, std::size_t(5));
      nodeId2columns.insert(node5, std::size_t(1));

      gum::learning::DirichletPriorFromDatabase prior(database, parser, nodeId2columns);

      TS_ASSERT_EQUALS(prior.weight(), 1.0)
      prior.setWeight(2.0 * db_size);
      TS_ASSERT_EQUALS(prior.weight(), 2.0 * db_size)

      TS_ASSERT_EQUALS(prior.getType(), gum::learning::PriorType::DirichletPriorType)

      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond1{node1};

      gum::learning::IdCondSet idset1(node2, cond_empty);                // #3,#0
      gum::learning::IdCondSet idset2(node2, node5, cond_empty, true);   // #9,#0
      gum::learning::IdCondSet idset3(node2, cond1, true);               // #9,#0


      std::vector< double > vect(3, 1.0);
      prior.addJointPseudoCount(idset1, vect);
      TS_ASSERT_EQUALS(vect[0], 2401.0)
      TS_ASSERT_EQUALS(vect[1], 251.0)
      TS_ASSERT_EQUALS(vect[2], 151.0)

      vect.clear();
      prior.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior.addJointPseudoCount(idset2, vect);
      TS_ASSERT_EQUALS(vect[0], 401.0)    // 0,0
      TS_ASSERT_EQUALS(vect[1], 151.0)    // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 2001.0)   // 0,1
      TS_ASSERT_EQUALS(vect[4], 1.0)      // 1,1
      TS_ASSERT_EQUALS(vect[5], 151.0)    // 2,1
      TS_ASSERT_EQUALS(vect[6], 1.0)      // 0,2
      TS_ASSERT_EQUALS(vect[7], 101.0)    // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      prior.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior.addJointPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 1.0)      // 0,0
      TS_ASSERT_EQUALS(vect[1], 151.0)    // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 401.0)    // 0,1
      TS_ASSERT_EQUALS(vect[4], 101.0)    // 1,1
      TS_ASSERT_EQUALS(vect[5], 151.0)    // 2,1
      TS_ASSERT_EQUALS(vect[6], 2001.0)   // 0,2
      TS_ASSERT_EQUALS(vect[7], 1.0)      // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      prior.addConditioningPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 151.0)
      TS_ASSERT_EQUALS(vect[1], 651.0)
      TS_ASSERT_EQUALS(vect[2], 2001.0)


      gum::learning::DirichletPriorFromDatabase prior2(prior);
      TS_ASSERT_EQUALS(prior2.weight(), 2.0 * db_size)
      prior2.setWeight(1.0 * db_size);
      TS_ASSERT_EQUALS(prior2.weight(), 1.0 * db_size)

      TS_ASSERT_EQUALS(prior2.getType(), gum::learning::PriorType::DirichletPriorType)

      vect.clear();
      vect.resize(3, 1.0);
      prior2.addJointPseudoCount(idset1, vect);
      TS_ASSERT_EQUALS(vect[0], 1201.0)
      TS_ASSERT_EQUALS(vect[1], 126.0)
      TS_ASSERT_EQUALS(vect[2], 76.0)

      vect.clear();
      prior2.addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior2.addJointPseudoCount(idset2, vect);
      TS_ASSERT_EQUALS(vect[0], 201.0)    // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 1001.0)   // 0,1
      TS_ASSERT_EQUALS(vect[4], 1.0)      // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1.0)      // 0,2
      TS_ASSERT_EQUALS(vect[7], 51.0)     // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      prior2.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior2.addJointPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 1.0)      // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 201.0)    // 0,1
      TS_ASSERT_EQUALS(vect[4], 51.0)     // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1001.0)   // 0,2
      TS_ASSERT_EQUALS(vect[7], 1.0)      // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      prior2.addConditioningPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 76.0)
      TS_ASSERT_EQUALS(vect[1], 326.0)
      TS_ASSERT_EQUALS(vect[2], 1001.0)


      gum::learning::DirichletPriorFromDatabase prior3(std::move(prior2));
      TS_ASSERT_EQUALS(prior3.weight(), 1.0 * db_size)
      prior3.setWeight(2.0 * db_size);
      TS_ASSERT_EQUALS(prior3.weight(), 2.0 * db_size)

      vect.clear();
      vect.resize(3, 1.0);
      prior3.addJointPseudoCount(idset1, vect);
      TS_ASSERT_EQUALS(vect[0], 2401.0)
      TS_ASSERT_EQUALS(vect[1], 251.0)
      TS_ASSERT_EQUALS(vect[2], 151.0)

      vect.clear();
      prior3.addConditioningPseudoCount(idset1, vect);
      TS_ASSERT_EQUALS(vect.size(), std::size_t(0))

      vect.clear();
      vect.resize(9, 1.0);
      prior3.addJointPseudoCount(idset2, vect);
      TS_ASSERT_EQUALS(vect[0], 401.0)    // 0,0
      TS_ASSERT_EQUALS(vect[1], 151.0)    // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 2001.0)   // 0,1
      TS_ASSERT_EQUALS(vect[4], 1.0)      // 1,1
      TS_ASSERT_EQUALS(vect[5], 151.0)    // 2,1
      TS_ASSERT_EQUALS(vect[6], 1.0)      // 0,2
      TS_ASSERT_EQUALS(vect[7], 101.0)    // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      prior3.addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior3.addJointPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 1.0)      // 0,0
      TS_ASSERT_EQUALS(vect[1], 151.0)    // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 401.0)    // 0,1
      TS_ASSERT_EQUALS(vect[4], 101.0)    // 1,1
      TS_ASSERT_EQUALS(vect[5], 151.0)    // 2,1
      TS_ASSERT_EQUALS(vect[6], 2001.0)   // 0,2
      TS_ASSERT_EQUALS(vect[7], 1.0)      // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      prior3.addConditioningPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 151.0)
      TS_ASSERT_EQUALS(vect[1], 651.0)
      TS_ASSERT_EQUALS(vect[2], 2001.0)


      gum::learning::DirichletPriorFromDatabase* prior4 = prior3.clone();
      TS_ASSERT_EQUALS(prior4->weight(), 2.0 * db_size)
      prior4->setWeight(1.0 * db_size);
      TS_ASSERT_EQUALS(prior4->weight(), 1.0 * db_size)

      TS_ASSERT_EQUALS(prior4->getType(), gum::learning::PriorType::DirichletPriorType)

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addJointPseudoCount(idset1, vect);
      TS_ASSERT_EQUALS(vect[0], 1201.0)
      TS_ASSERT_EQUALS(vect[1], 126.0)
      TS_ASSERT_EQUALS(vect[2], 76.0)

      vect.clear();
      prior4->addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior4->addJointPseudoCount(idset2, vect);
      TS_ASSERT_EQUALS(vect[0], 201.0)    // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 1001.0)   // 0,1
      TS_ASSERT_EQUALS(vect[4], 1.0)      // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1.0)      // 0,2
      TS_ASSERT_EQUALS(vect[7], 51.0)     // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      prior4->addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior4->addJointPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 1.0)      // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 201.0)    // 0,1
      TS_ASSERT_EQUALS(vect[4], 51.0)     // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1001.0)   // 0,2
      TS_ASSERT_EQUALS(vect[7], 1.0)      // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addConditioningPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 76.0)
      TS_ASSERT_EQUALS(vect[1], 326.0)
      TS_ASSERT_EQUALS(vect[2], 1001.0)


      prior4->operator=(prior);
      TS_ASSERT_EQUALS(prior4->weight(), 2.0 * db_size)
      prior4->setWeight(1.0 * db_size);
      TS_ASSERT_EQUALS(prior4->weight(), 1.0 * db_size)

      TS_ASSERT_EQUALS(prior4->getType(), gum::learning::PriorType::DirichletPriorType)

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addJointPseudoCount(idset1, vect);
      TS_ASSERT_EQUALS(vect[0], 1201.0)
      TS_ASSERT_EQUALS(vect[1], 126.0)
      TS_ASSERT_EQUALS(vect[2], 76.0)

      vect.clear();
      prior4->addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior4->addJointPseudoCount(idset2, vect);
      TS_ASSERT_EQUALS(vect[0], 201.0)    // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 1001.0)   // 0,1
      TS_ASSERT_EQUALS(vect[4], 1.0)      // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1.0)      // 0,2
      TS_ASSERT_EQUALS(vect[7], 51.0)     // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      prior4->addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior4->addJointPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 1.0)      // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 201.0)    // 0,1
      TS_ASSERT_EQUALS(vect[4], 51.0)     // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1001.0)   // 0,2
      TS_ASSERT_EQUALS(vect[7], 1.0)      // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addConditioningPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 76.0)
      TS_ASSERT_EQUALS(vect[1], 326.0)
      TS_ASSERT_EQUALS(vect[2], 1001.0)


      prior4->operator=(std::move(prior));
      TS_ASSERT_EQUALS(prior4->weight(), 2.0 * db_size)
      prior4->setWeight(1.0 * db_size);
      TS_ASSERT_EQUALS(prior4->weight(), 1.0 * db_size)

      TS_ASSERT_EQUALS(prior4->getType(), gum::learning::PriorType::DirichletPriorType)

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addJointPseudoCount(idset1, vect);
      TS_ASSERT_EQUALS(vect[0], 1201.0)
      TS_ASSERT_EQUALS(vect[1], 126.0)
      TS_ASSERT_EQUALS(vect[2], 76.0)

      vect.clear();
      prior4->addConditioningPseudoCount(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior4->addJointPseudoCount(idset2, vect);
      TS_ASSERT_EQUALS(vect[0], 201.0)    // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 1001.0)   // 0,1
      TS_ASSERT_EQUALS(vect[4], 1.0)      // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1.0)      // 0,2
      TS_ASSERT_EQUALS(vect[7], 51.0)     // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      prior4->addConditioningPseudoCount(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      prior4->addJointPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 1.0)      // 0,0
      TS_ASSERT_EQUALS(vect[1], 76.0)     // 1,0
      TS_ASSERT_EQUALS(vect[2], 1.0)      // 2,0
      TS_ASSERT_EQUALS(vect[3], 201.0)    // 0,1
      TS_ASSERT_EQUALS(vect[4], 51.0)     // 1,1
      TS_ASSERT_EQUALS(vect[5], 76.0)     // 2,1
      TS_ASSERT_EQUALS(vect[6], 1001.0)   // 0,2
      TS_ASSERT_EQUALS(vect[7], 1.0)      // 1,2
      TS_ASSERT_EQUALS(vect[8], 1.0)      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      prior4->addConditioningPseudoCount(idset3, vect);
      TS_ASSERT_EQUALS(vect[0], 76.0)
      TS_ASSERT_EQUALS(vect[1], 326.0)
      TS_ASSERT_EQUALS(vect[2], 1001.0)

      delete prior4;
    }
  };


} /* namespace gum_tests */
