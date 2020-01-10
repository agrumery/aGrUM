
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
#include <agrum/learning/aprioris/aprioriDirichletFromDatabase.h>

namespace gum_tests {

  class AprioriDirichletFromDatabaseTestSuite: public CxxTest::TestSuite {
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
      const std::size_t db_size = database.nbRows();

      // create the parser
      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);


      gum::learning::AprioriDirichletFromDatabase<> apriori(database, parser);

      TS_ASSERT(apriori.weight() == 1.0);
      apriori.setWeight(2.0 * db_size);
      TS_ASSERT(apriori.weight() == 2.0 * db_size);

      TS_ASSERT(apriori.getType() == "Dirichlet");
      TS_ASSERT(apriori.isOfType("Dirichlet"));
      TS_ASSERT(!apriori.isOfType("NoApriori"));

      gum::NodeId                node0 = 0;
      gum::NodeId                node1 = 1;
      gum::NodeId                node3 = 3;
      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond1{node3};

      gum::learning::IdSet<> idset1(node0, cond_empty);                // #3,#0
      gum::learning::IdSet<> idset2(node0, node1, cond_empty, true);   // #9,#0
      gum::learning::IdSet<> idset3(node0, cond1, true);               // #9,#0


      std::vector< double > vect(3, 1.0);
      apriori.addAllApriori(idset1, vect);
      TS_ASSERT(vect[0] == 2401.0);
      TS_ASSERT(vect[1] == 251.0);
      TS_ASSERT(vect[2] == 151.0);

      vect.clear();
      apriori.addConditioningApriori(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori.addAllApriori(idset2, vect);
      TS_ASSERT(vect[0] == 401.0);    // 0,0
      TS_ASSERT(vect[1] == 151.0);    // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 2001.0);   // 0,1
      TS_ASSERT(vect[4] == 1.0);      // 1,1
      TS_ASSERT(vect[5] == 151.0);    // 2,1
      TS_ASSERT(vect[6] == 1.0);      // 0,2
      TS_ASSERT(vect[7] == 101.0);    // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      apriori.addConditioningApriori(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori.addAllApriori(idset3, vect);
      TS_ASSERT(vect[0] == 1.0);      // 0,0
      TS_ASSERT(vect[1] == 151.0);    // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 401.0);    // 0,1
      TS_ASSERT(vect[4] == 101.0);    // 1,1
      TS_ASSERT(vect[5] == 151.0);    // 2,1
      TS_ASSERT(vect[6] == 2001.0);   // 0,2
      TS_ASSERT(vect[7] == 1.0);      // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      apriori.addConditioningApriori(idset3, vect);
      TS_ASSERT(vect[0] == 151.0);
      TS_ASSERT(vect[1] == 651.0);
      TS_ASSERT(vect[2] == 2001.0);


      gum::learning::AprioriDirichletFromDatabase<> apriori2(apriori);
      TS_ASSERT(apriori2.weight() == 2.0 * db_size);
      apriori2.setWeight(1.0 * db_size);
      TS_ASSERT(apriori2.weight() == db_size);

      TS_ASSERT(apriori2.getType() == "Dirichlet");
      TS_ASSERT(apriori2.isOfType("Dirichlet"));
      TS_ASSERT(!apriori2.isOfType("NoApriori"));

      vect.clear();
      vect.resize(3, 1.0);
      apriori2.addAllApriori(idset1, vect);
      TS_ASSERT(vect[0] == 1201.0);
      TS_ASSERT(vect[1] == 126.0);
      TS_ASSERT(vect[2] == 76.0);

      vect.clear();
      apriori2.addConditioningApriori(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori2.addAllApriori(idset2, vect);
      TS_ASSERT(vect[0] == 201.0);    // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 1001.0);   // 0,1
      TS_ASSERT(vect[4] == 1.0);      // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1.0);      // 0,2
      TS_ASSERT(vect[7] == 51.0);     // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      apriori2.addConditioningApriori(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori2.addAllApriori(idset3, vect);
      TS_ASSERT(vect[0] == 1.0);      // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 201.0);    // 0,1
      TS_ASSERT(vect[4] == 51.0);     // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1001.0);   // 0,2
      TS_ASSERT(vect[7] == 1.0);      // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      apriori2.addConditioningApriori(idset3, vect);
      TS_ASSERT(vect[0] == 76.0);
      TS_ASSERT(vect[1] == 326.0);
      TS_ASSERT(vect[2] == 1001.0);


      gum::learning::AprioriDirichletFromDatabase<> apriori3(std::move(apriori2));
      TS_ASSERT(apriori3.weight() == db_size);
      apriori3.setWeight(2.0 * db_size);
      TS_ASSERT(apriori3.weight() == 2.0 * db_size);

      vect.clear();
      vect.resize(3, 1.0);
      apriori3.addAllApriori(idset1, vect);
      TS_ASSERT(vect[0] == 2401.0);
      TS_ASSERT(vect[1] == 251.0);
      TS_ASSERT(vect[2] == 151.0);

      vect.clear();
      apriori3.addConditioningApriori(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori3.addAllApriori(idset2, vect);
      TS_ASSERT(vect[0] == 401.0);    // 0,0
      TS_ASSERT(vect[1] == 151.0);    // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 2001.0);   // 0,1
      TS_ASSERT(vect[4] == 1.0);      // 1,1
      TS_ASSERT(vect[5] == 151.0);    // 2,1
      TS_ASSERT(vect[6] == 1.0);      // 0,2
      TS_ASSERT(vect[7] == 101.0);    // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      apriori3.addConditioningApriori(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori3.addAllApriori(idset3, vect);
      TS_ASSERT(vect[0] == 1.0);      // 0,0
      TS_ASSERT(vect[1] == 151.0);    // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 401.0);    // 0,1
      TS_ASSERT(vect[4] == 101.0);    // 1,1
      TS_ASSERT(vect[5] == 151.0);    // 2,1
      TS_ASSERT(vect[6] == 2001.0);   // 0,2
      TS_ASSERT(vect[7] == 1.0);      // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      apriori3.addConditioningApriori(idset3, vect);
      TS_ASSERT(vect[0] == 151.0);
      TS_ASSERT(vect[1] == 651.0);
      TS_ASSERT(vect[2] == 2001.0);


      gum::learning::AprioriDirichletFromDatabase<>* apriori4 = apriori3.clone();
      TS_ASSERT(apriori4->weight() == 2.0 * db_size);
      apriori4->setWeight(1.0 * db_size);
      TS_ASSERT(apriori4->weight() == 1.0 * db_size);

      TS_ASSERT(apriori4->getType() == "Dirichlet");
      TS_ASSERT(apriori4->isOfType("Dirichlet"));
      TS_ASSERT(!apriori4->isOfType("NoApriori"));

      vect.clear();
      vect.resize(3, 1.0);
      apriori4->addAllApriori(idset1, vect);
      TS_ASSERT(vect[0] == 1201.0);
      TS_ASSERT(vect[1] == 126.0);
      TS_ASSERT(vect[2] == 76.0);

      vect.clear();
      apriori4->addConditioningApriori(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori4->addAllApriori(idset2, vect);
      TS_ASSERT(vect[0] == 201.0);    // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 1001.0);   // 0,1
      TS_ASSERT(vect[4] == 1.0);      // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1.0);      // 0,2
      TS_ASSERT(vect[7] == 51.0);     // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      apriori4->addConditioningApriori(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori4->addAllApriori(idset3, vect);
      TS_ASSERT(vect[0] == 1.0);      // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 201.0);    // 0,1
      TS_ASSERT(vect[4] == 51.0);     // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1001.0);   // 0,2
      TS_ASSERT(vect[7] == 1.0);      // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      apriori4->addConditioningApriori(idset3, vect);
      TS_ASSERT(vect[0] == 76.0);
      TS_ASSERT(vect[1] == 326.0);
      TS_ASSERT(vect[2] == 1001.0);


      apriori4->operator=(apriori);
      TS_ASSERT(apriori4->weight() == 2.0 * db_size);
      apriori4->setWeight(1.0 * db_size);
      TS_ASSERT(apriori4->weight() == 1.0 * db_size);

      TS_ASSERT(apriori4->getType() == "Dirichlet");
      TS_ASSERT(apriori4->isOfType("Dirichlet"));
      TS_ASSERT(!apriori4->isOfType("NoApriori"));

      vect.clear();
      vect.resize(3, 1.0);
      apriori4->addAllApriori(idset1, vect);
      TS_ASSERT(vect[0] == 1201.0);
      TS_ASSERT(vect[1] == 126.0);
      TS_ASSERT(vect[2] == 76.0);

      vect.clear();
      apriori4->addConditioningApriori(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori4->addAllApriori(idset2, vect);
      TS_ASSERT(vect[0] == 201.0);    // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 1001.0);   // 0,1
      TS_ASSERT(vect[4] == 1.0);      // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1.0);      // 0,2
      TS_ASSERT(vect[7] == 51.0);     // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      apriori4->addConditioningApriori(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori4->addAllApriori(idset3, vect);
      TS_ASSERT(vect[0] == 1.0);      // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 201.0);    // 0,1
      TS_ASSERT(vect[4] == 51.0);     // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1001.0);   // 0,2
      TS_ASSERT(vect[7] == 1.0);      // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      apriori4->addConditioningApriori(idset3, vect);
      TS_ASSERT(vect[0] == 76.0);
      TS_ASSERT(vect[1] == 326.0);
      TS_ASSERT(vect[2] == 1001.0);


      apriori4->operator=(std::move(apriori));
      TS_ASSERT(apriori4->weight() == 2.0 * db_size);
      apriori4->setWeight(1.0 * db_size);
      TS_ASSERT(apriori4->weight() == 1.0 * db_size);

      TS_ASSERT(apriori4->getType() == "Dirichlet");
      TS_ASSERT(apriori4->isOfType("Dirichlet"));
      TS_ASSERT(!apriori4->isOfType("NoApriori"));

      vect.clear();
      vect.resize(3, 1.0);
      apriori4->addAllApriori(idset1, vect);
      TS_ASSERT(vect[0] == 1201.0);
      TS_ASSERT(vect[1] == 126.0);
      TS_ASSERT(vect[2] == 76.0);

      vect.clear();
      apriori4->addConditioningApriori(idset1, vect);


      vect.clear();
      vect.resize(9, 1.0);
      apriori4->addAllApriori(idset2, vect);
      TS_ASSERT(vect[0] == 201.0);    // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 1001.0);   // 0,1
      TS_ASSERT(vect[4] == 1.0);      // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1.0);      // 0,2
      TS_ASSERT(vect[7] == 51.0);     // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      apriori4->addConditioningApriori(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori4->addAllApriori(idset3, vect);
      TS_ASSERT(vect[0] == 1.0);      // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 201.0);    // 0,1
      TS_ASSERT(vect[4] == 51.0);     // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1001.0);   // 0,2
      TS_ASSERT(vect[7] == 1.0);      // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      apriori4->addConditioningApriori(idset3, vect);
      TS_ASSERT(vect[0] == 76.0);
      TS_ASSERT(vect[1] == 326.0);
      TS_ASSERT(vect[2] == 1001.0);

      delete apriori4;
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

      const std::size_t db_size = database.nbRows();

      // create the parser
      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);

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

      gum::learning::AprioriDirichletFromDatabase<> apriori(
         database, parser, nodeId2columns);

      TS_ASSERT(apriori.weight() == 1.0);
      apriori.setWeight(2.0 * db_size);
      TS_ASSERT(apriori.weight() == 2.0 * db_size);

      TS_ASSERT(apriori.getType() == "Dirichlet");
      TS_ASSERT(apriori.isOfType("Dirichlet"));
      TS_ASSERT(!apriori.isOfType("NoApriori"));

      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond1{node1};

      gum::learning::IdSet<> idset1(node2, cond_empty);                // #3,#0
      gum::learning::IdSet<> idset2(node2, node5, cond_empty, true);   // #9,#0
      gum::learning::IdSet<> idset3(node2, cond1, true);               // #9,#0


      std::vector< double > vect(3, 1.0);
      apriori.addAllApriori(idset1, vect);
      TS_ASSERT(vect[0] == 2401.0);
      TS_ASSERT(vect[1] == 251.0);
      TS_ASSERT(vect[2] == 151.0);

      vect.clear();
      apriori.addConditioningApriori(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori.addAllApriori(idset2, vect);
      TS_ASSERT(vect[0] == 401.0);    // 0,0
      TS_ASSERT(vect[1] == 151.0);    // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 2001.0);   // 0,1
      TS_ASSERT(vect[4] == 1.0);      // 1,1
      TS_ASSERT(vect[5] == 151.0);    // 2,1
      TS_ASSERT(vect[6] == 1.0);      // 0,2
      TS_ASSERT(vect[7] == 101.0);    // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      apriori.addConditioningApriori(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori.addAllApriori(idset3, vect);
      TS_ASSERT(vect[0] == 1.0);      // 0,0
      TS_ASSERT(vect[1] == 151.0);    // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 401.0);    // 0,1
      TS_ASSERT(vect[4] == 101.0);    // 1,1
      TS_ASSERT(vect[5] == 151.0);    // 2,1
      TS_ASSERT(vect[6] == 2001.0);   // 0,2
      TS_ASSERT(vect[7] == 1.0);      // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      apriori.addConditioningApriori(idset3, vect);
      TS_ASSERT(vect[0] == 151.0);
      TS_ASSERT(vect[1] == 651.0);
      TS_ASSERT(vect[2] == 2001.0);


      gum::learning::AprioriDirichletFromDatabase<> apriori2(apriori);
      TS_ASSERT(apriori2.weight() == 2.0 * db_size);
      apriori2.setWeight(1.0 * db_size);
      TS_ASSERT(apriori2.weight() == 1.0 * db_size);

      TS_ASSERT(apriori2.getType() == "Dirichlet");
      TS_ASSERT(apriori2.isOfType("Dirichlet"));
      TS_ASSERT(!apriori2.isOfType("NoApriori"));

      vect.clear();
      vect.resize(3, 1.0);
      apriori2.addAllApriori(idset1, vect);
      TS_ASSERT(vect[0] == 1201.0);
      TS_ASSERT(vect[1] == 126.0);
      TS_ASSERT(vect[2] == 76.0);

      vect.clear();
      apriori2.addConditioningApriori(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori2.addAllApriori(idset2, vect);
      TS_ASSERT(vect[0] == 201.0);    // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 1001.0);   // 0,1
      TS_ASSERT(vect[4] == 1.0);      // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1.0);      // 0,2
      TS_ASSERT(vect[7] == 51.0);     // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      apriori2.addConditioningApriori(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori2.addAllApriori(idset3, vect);
      TS_ASSERT(vect[0] == 1.0);      // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 201.0);    // 0,1
      TS_ASSERT(vect[4] == 51.0);     // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1001.0);   // 0,2
      TS_ASSERT(vect[7] == 1.0);      // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      apriori2.addConditioningApriori(idset3, vect);
      TS_ASSERT(vect[0] == 76.0);
      TS_ASSERT(vect[1] == 326.0);
      TS_ASSERT(vect[2] == 1001.0);


      gum::learning::AprioriDirichletFromDatabase<> apriori3(std::move(apriori2));
      TS_ASSERT(apriori3.weight() == 1.0 * db_size);
      apriori3.setWeight(2.0 * db_size);
      TS_ASSERT(apriori3.weight() == 2.0 * db_size);

      vect.clear();
      vect.resize(3, 1.0);
      apriori3.addAllApriori(idset1, vect);
      TS_ASSERT(vect[0] == 2401.0);
      TS_ASSERT(vect[1] == 251.0);
      TS_ASSERT(vect[2] == 151.0);

      vect.clear();
      apriori3.addConditioningApriori(idset1, vect);
      TS_ASSERT(vect.size() == std::size_t(0));

      vect.clear();
      vect.resize(9, 1.0);
      apriori3.addAllApriori(idset2, vect);
      TS_ASSERT(vect[0] == 401.0);    // 0,0
      TS_ASSERT(vect[1] == 151.0);    // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 2001.0);   // 0,1
      TS_ASSERT(vect[4] == 1.0);      // 1,1
      TS_ASSERT(vect[5] == 151.0);    // 2,1
      TS_ASSERT(vect[6] == 1.0);      // 0,2
      TS_ASSERT(vect[7] == 101.0);    // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      apriori3.addConditioningApriori(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori3.addAllApriori(idset3, vect);
      TS_ASSERT(vect[0] == 1.0);      // 0,0
      TS_ASSERT(vect[1] == 151.0);    // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 401.0);    // 0,1
      TS_ASSERT(vect[4] == 101.0);    // 1,1
      TS_ASSERT(vect[5] == 151.0);    // 2,1
      TS_ASSERT(vect[6] == 2001.0);   // 0,2
      TS_ASSERT(vect[7] == 1.0);      // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      apriori3.addConditioningApriori(idset3, vect);
      TS_ASSERT(vect[0] == 151.0);
      TS_ASSERT(vect[1] == 651.0);
      TS_ASSERT(vect[2] == 2001.0);


      gum::learning::AprioriDirichletFromDatabase<>* apriori4 = apriori3.clone();
      TS_ASSERT(apriori4->weight() == 2.0 * db_size);
      apriori4->setWeight(1.0 * db_size);
      TS_ASSERT(apriori4->weight() == 1.0 * db_size);

      TS_ASSERT(apriori4->getType() == "Dirichlet");
      TS_ASSERT(apriori4->isOfType("Dirichlet"));
      TS_ASSERT(!apriori4->isOfType("NoApriori"));

      vect.clear();
      vect.resize(3, 1.0);
      apriori4->addAllApriori(idset1, vect);
      TS_ASSERT(vect[0] == 1201.0);
      TS_ASSERT(vect[1] == 126.0);
      TS_ASSERT(vect[2] == 76.0);

      vect.clear();
      apriori4->addConditioningApriori(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori4->addAllApriori(idset2, vect);
      TS_ASSERT(vect[0] == 201.0);    // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 1001.0);   // 0,1
      TS_ASSERT(vect[4] == 1.0);      // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1.0);      // 0,2
      TS_ASSERT(vect[7] == 51.0);     // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      apriori4->addConditioningApriori(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori4->addAllApriori(idset3, vect);
      TS_ASSERT(vect[0] == 1.0);      // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 201.0);    // 0,1
      TS_ASSERT(vect[4] == 51.0);     // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1001.0);   // 0,2
      TS_ASSERT(vect[7] == 1.0);      // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      apriori4->addConditioningApriori(idset3, vect);
      TS_ASSERT(vect[0] == 76.0);
      TS_ASSERT(vect[1] == 326.0);
      TS_ASSERT(vect[2] == 1001.0);


      apriori4->operator=(apriori);
      TS_ASSERT(apriori4->weight() == 2.0 * db_size);
      apriori4->setWeight(1.0 * db_size);
      TS_ASSERT(apriori4->weight() == 1.0 * db_size);

      TS_ASSERT(apriori4->getType() == "Dirichlet");
      TS_ASSERT(apriori4->isOfType("Dirichlet"));
      TS_ASSERT(!apriori4->isOfType("NoApriori"));

      vect.clear();
      vect.resize(3, 1.0);
      apriori4->addAllApriori(idset1, vect);
      TS_ASSERT(vect[0] == 1201.0);
      TS_ASSERT(vect[1] == 126.0);
      TS_ASSERT(vect[2] == 76.0);

      vect.clear();
      apriori4->addConditioningApriori(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori4->addAllApriori(idset2, vect);
      TS_ASSERT(vect[0] == 201.0);    // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 1001.0);   // 0,1
      TS_ASSERT(vect[4] == 1.0);      // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1.0);      // 0,2
      TS_ASSERT(vect[7] == 51.0);     // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      apriori4->addConditioningApriori(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori4->addAllApriori(idset3, vect);
      TS_ASSERT(vect[0] == 1.0);      // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 201.0);    // 0,1
      TS_ASSERT(vect[4] == 51.0);     // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1001.0);   // 0,2
      TS_ASSERT(vect[7] == 1.0);      // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      apriori4->addConditioningApriori(idset3, vect);
      TS_ASSERT(vect[0] == 76.0);
      TS_ASSERT(vect[1] == 326.0);
      TS_ASSERT(vect[2] == 1001.0);


      apriori4->operator=(std::move(apriori));
      TS_ASSERT(apriori4->weight() == 2.0 * db_size);
      apriori4->setWeight(1.0 * db_size);
      TS_ASSERT(apriori4->weight() == 1.0 * db_size);

      TS_ASSERT(apriori4->getType() == "Dirichlet");
      TS_ASSERT(apriori4->isOfType("Dirichlet"));
      TS_ASSERT(!apriori4->isOfType("NoApriori"));

      vect.clear();
      vect.resize(3, 1.0);
      apriori4->addAllApriori(idset1, vect);
      TS_ASSERT(vect[0] == 1201.0);
      TS_ASSERT(vect[1] == 126.0);
      TS_ASSERT(vect[2] == 76.0);

      vect.clear();
      apriori4->addConditioningApriori(idset1, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori4->addAllApriori(idset2, vect);
      TS_ASSERT(vect[0] == 201.0);    // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 1001.0);   // 0,1
      TS_ASSERT(vect[4] == 1.0);      // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1.0);      // 0,2
      TS_ASSERT(vect[7] == 51.0);     // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      apriori4->addConditioningApriori(idset2, vect);

      vect.clear();
      vect.resize(9, 1.0);
      apriori4->addAllApriori(idset3, vect);
      TS_ASSERT(vect[0] == 1.0);      // 0,0
      TS_ASSERT(vect[1] == 76.0);     // 1,0
      TS_ASSERT(vect[2] == 1.0);      // 2,0
      TS_ASSERT(vect[3] == 201.0);    // 0,1
      TS_ASSERT(vect[4] == 51.0);     // 1,1
      TS_ASSERT(vect[5] == 76.0);     // 2,1
      TS_ASSERT(vect[6] == 1001.0);   // 0,2
      TS_ASSERT(vect[7] == 1.0);      // 1,2
      TS_ASSERT(vect[8] == 1.0);      // 2,2

      vect.clear();
      vect.resize(3, 1.0);
      apriori4->addConditioningApriori(idset3, vect);
      TS_ASSERT(vect[0] == 76.0);
      TS_ASSERT(vect[1] == 326.0);
      TS_ASSERT(vect[2] == 1001.0);

      delete apriori4;
    }
  };


} /* namespace gum_tests */
