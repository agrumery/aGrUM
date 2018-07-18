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
#include <agrum/learning/aprioris/aprioriDirichletFromDatabase2.h>

namespace gum_tests {

  class AprioriDirichletFromDatabase2TestSuite : public CxxTest::TestSuite {
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


      gum::learning::AprioriDirichletFromDatabase2<> apriori(database, parser);

      TS_ASSERT(apriori.weight() == 1.0);
      apriori.setWeight(2.0);
      TS_ASSERT(apriori.weight() == 2.0);

      TS_ASSERT(apriori.getType() == "Dirichlet");
      TS_ASSERT(apriori.isOfType("Dirichlet"));
      TS_ASSERT(!apriori.isOfType("NoApriori"));

      gum::NodeId                node0 = 0;
      gum::NodeId                node1 = 1;
      gum::NodeId                node3 = 3;
      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond1{node3};

      gum::learning::IdSet2<> idset1(node0, cond_empty);                // #3,#0
      gum::learning::IdSet2<> idset2(node0, node1, cond_empty, true);   // #9,#0
      gum::learning::IdSet2<> idset3(node0, cond1, true);               // #9,#0


      std::vector< double > vect = apriori.getAllApriori(idset1);
      TS_ASSERT(vect.size() == std::size_t(3));
      TS_ASSERT(vect[0] == 2400.0);
      TS_ASSERT(vect[1] == 250.0);
      TS_ASSERT(vect[2] == 150.0);

      vect = apriori.getConditioningApriori(idset1);
      TS_ASSERT(vect.size() == std::size_t(0));

      vect = apriori.getAllApriori(idset2);
      TS_ASSERT(vect.size() == std::size_t(9));
      TS_ASSERT(vect[0] == 400.0);    // 0,0
      TS_ASSERT(vect[1] == 150.0);    // 1,0
      TS_ASSERT(vect[2] == 0.0);      // 2,0
      TS_ASSERT(vect[3] == 2000.0);   // 0,1
      TS_ASSERT(vect[4] == 0.0);      // 1,1
      TS_ASSERT(vect[5] == 150.0);    // 2,1
      TS_ASSERT(vect[6] == 0.0);      // 0,2
      TS_ASSERT(vect[7] == 100.0);    // 1,2
      TS_ASSERT(vect[8] == 0.0);      // 2,2

      vect = apriori.getConditioningApriori(idset2);
      TS_ASSERT(vect.size() == std::size_t(0));

      vect = apriori.getAllApriori(idset3);
      TS_ASSERT(vect.size() == std::size_t(9));
      TS_ASSERT(vect[0] == 0.0);      // 0,0
      TS_ASSERT(vect[1] == 150.0);    // 1,0
      TS_ASSERT(vect[2] == 0.0);      // 2,0
      TS_ASSERT(vect[3] == 400.0);    // 0,1
      TS_ASSERT(vect[4] == 100.0);    // 1,1
      TS_ASSERT(vect[5] == 150.0);    // 2,1
      TS_ASSERT(vect[6] == 2000.0);   // 0,2
      TS_ASSERT(vect[7] == 0.0);      // 1,2
      TS_ASSERT(vect[8] == 0.0);      // 2,2

      vect = apriori.getConditioningApriori(idset3);
      TS_ASSERT(vect.size() == std::size_t(3));
      TS_ASSERT(vect[0] == 150.0);
      TS_ASSERT(vect[1] == 650.0);
      TS_ASSERT(vect[2] == 2000.0);


      gum::learning::AprioriDirichletFromDatabase2<> apriori2(apriori);
      TS_ASSERT(apriori2.weight() == 2.0);
      apriori2.setWeight(1.0);
      TS_ASSERT(apriori2.weight() == 1.0);

      TS_ASSERT(apriori2.getType() == "Dirichlet");
      TS_ASSERT(apriori2.isOfType("Dirichlet"));
      TS_ASSERT(!apriori2.isOfType("NoApriori"));

      vect = apriori2.getAllApriori(idset1);
      TS_ASSERT(vect.size() == std::size_t(3));
      TS_ASSERT(vect[0] == 1200.0);
      TS_ASSERT(vect[1] == 125.0);
      TS_ASSERT(vect[2] == 75.0);

      vect = apriori2.getConditioningApriori(idset1);
      TS_ASSERT(vect.size() == std::size_t(0));

      vect = apriori2.getAllApriori(idset2);
      TS_ASSERT(vect.size() == std::size_t(9));
      TS_ASSERT(vect[0] == 200.0);    // 0,0
      TS_ASSERT(vect[1] == 75.0);     // 1,0
      TS_ASSERT(vect[2] == 0.0);      // 2,0
      TS_ASSERT(vect[3] == 1000.0);   // 0,1
      TS_ASSERT(vect[4] == 0.0);      // 1,1
      TS_ASSERT(vect[5] == 75.0);     // 2,1
      TS_ASSERT(vect[6] == 0.0);      // 0,2
      TS_ASSERT(vect[7] == 50.0);     // 1,2
      TS_ASSERT(vect[8] == 0.0);      // 2,2

      vect = apriori2.getConditioningApriori(idset2);
      TS_ASSERT(vect.size() == std::size_t(0));

      vect = apriori2.getAllApriori(idset3);
      TS_ASSERT(vect.size() == std::size_t(9));
      TS_ASSERT(vect[0] == 0.0);      // 0,0
      TS_ASSERT(vect[1] == 75.0);     // 1,0
      TS_ASSERT(vect[2] == 0.0);      // 2,0
      TS_ASSERT(vect[3] == 200.0);    // 0,1
      TS_ASSERT(vect[4] == 50.0);     // 1,1
      TS_ASSERT(vect[5] == 75.0);     // 2,1
      TS_ASSERT(vect[6] == 1000.0);   // 0,2
      TS_ASSERT(vect[7] == 0.0);      // 1,2
      TS_ASSERT(vect[8] == 0.0);      // 2,2

      vect = apriori2.getConditioningApriori(idset3);
      TS_ASSERT(vect.size() == std::size_t(3));
      TS_ASSERT(vect[0] == 75.0);
      TS_ASSERT(vect[1] == 325.0);
      TS_ASSERT(vect[2] == 1000.0);


      gum::learning::AprioriDirichletFromDatabase2<> apriori3(std::move(apriori2));
      TS_ASSERT(apriori3.weight() == 1.0);
      apriori3.setWeight(2.0);
      TS_ASSERT(apriori3.weight() == 2.0);

      vect = apriori3.getAllApriori(idset1);
      TS_ASSERT(vect.size() == std::size_t(3));
      TS_ASSERT(vect[0] == 2400.0);
      TS_ASSERT(vect[1] == 250.0);
      TS_ASSERT(vect[2] == 150.0);

      vect = apriori3.getConditioningApriori(idset1);
      TS_ASSERT(vect.size() == std::size_t(0));

      vect = apriori3.getAllApriori(idset2);
      TS_ASSERT(vect.size() == std::size_t(9));
      TS_ASSERT(vect[0] == 400.0);    // 0,0
      TS_ASSERT(vect[1] == 150.0);    // 1,0
      TS_ASSERT(vect[2] == 0.0);      // 2,0
      TS_ASSERT(vect[3] == 2000.0);   // 0,1
      TS_ASSERT(vect[4] == 0.0);      // 1,1
      TS_ASSERT(vect[5] == 150.0);    // 2,1
      TS_ASSERT(vect[6] == 0.0);      // 0,2
      TS_ASSERT(vect[7] == 100.0);    // 1,2
      TS_ASSERT(vect[8] == 0.0);      // 2,2

      vect = apriori3.getConditioningApriori(idset2);
      TS_ASSERT(vect.size() == std::size_t(0));

      vect = apriori3.getAllApriori(idset3);
      TS_ASSERT(vect.size() == std::size_t(9));
      TS_ASSERT(vect[0] == 0.0);      // 0,0
      TS_ASSERT(vect[1] == 150.0);    // 1,0
      TS_ASSERT(vect[2] == 0.0);      // 2,0
      TS_ASSERT(vect[3] == 400.0);    // 0,1
      TS_ASSERT(vect[4] == 100.0);    // 1,1
      TS_ASSERT(vect[5] == 150.0);    // 2,1
      TS_ASSERT(vect[6] == 2000.0);   // 0,2
      TS_ASSERT(vect[7] == 0.0);      // 1,2
      TS_ASSERT(vect[8] == 0.0);      // 2,2

      vect = apriori3.getConditioningApriori(idset3);
      TS_ASSERT(vect.size() == std::size_t(3));
      TS_ASSERT(vect[0] == 150.0);
      TS_ASSERT(vect[1] == 650.0);
      TS_ASSERT(vect[2] == 2000.0);


      gum::learning::AprioriDirichletFromDatabase2<>* apriori4 = apriori3.clone();
      TS_ASSERT(apriori4->weight() == 2.0);
      apriori4->setWeight(1.0);
      TS_ASSERT(apriori4->weight() == 1.0);

      TS_ASSERT(apriori4->getType() == "Dirichlet");
      TS_ASSERT(apriori4->isOfType("Dirichlet"));
      TS_ASSERT(!apriori4->isOfType("NoApriori"));

      vect = apriori4->getAllApriori(idset1);
      TS_ASSERT(vect.size() == std::size_t(3));
      TS_ASSERT(vect[0] == 1200.0);
      TS_ASSERT(vect[1] == 125.0);
      TS_ASSERT(vect[2] == 75.0);

      vect = apriori4->getConditioningApriori(idset1);
      TS_ASSERT(vect.size() == std::size_t(0));

      vect = apriori4->getAllApriori(idset2);
      TS_ASSERT(vect.size() == std::size_t(9));
      TS_ASSERT(vect[0] == 200.0);    // 0,0
      TS_ASSERT(vect[1] == 75.0);     // 1,0
      TS_ASSERT(vect[2] == 0.0);      // 2,0
      TS_ASSERT(vect[3] == 1000.0);   // 0,1
      TS_ASSERT(vect[4] == 0.0);      // 1,1
      TS_ASSERT(vect[5] == 75.0);     // 2,1
      TS_ASSERT(vect[6] == 0.0);      // 0,2
      TS_ASSERT(vect[7] == 50.0);     // 1,2
      TS_ASSERT(vect[8] == 0.0);      // 2,2

      vect = apriori4->getConditioningApriori(idset2);
      TS_ASSERT(vect.size() == std::size_t(0));

      vect = apriori4->getAllApriori(idset3);
      TS_ASSERT(vect.size() == std::size_t(9));
      TS_ASSERT(vect[0] == 0.0);      // 0,0
      TS_ASSERT(vect[1] == 75.0);     // 1,0
      TS_ASSERT(vect[2] == 0.0);      // 2,0
      TS_ASSERT(vect[3] == 200.0);    // 0,1
      TS_ASSERT(vect[4] == 50.0);     // 1,1
      TS_ASSERT(vect[5] == 75.0);     // 2,1
      TS_ASSERT(vect[6] == 1000.0);   // 0,2
      TS_ASSERT(vect[7] == 0.0);      // 1,2
      TS_ASSERT(vect[8] == 0.0);      // 2,2

      vect = apriori4->getConditioningApriori(idset3);
      TS_ASSERT(vect.size() == std::size_t(3));
      TS_ASSERT(vect[0] == 75.0);
      TS_ASSERT(vect[1] == 325.0);
      TS_ASSERT(vect[2] == 1000.0);


      apriori4->operator=(apriori);
      TS_ASSERT(apriori4->weight() == 2.0);
      apriori4->setWeight(1.0);
      TS_ASSERT(apriori4->weight() == 1.0);

      TS_ASSERT(apriori4->getType() == "Dirichlet");
      TS_ASSERT(apriori4->isOfType("Dirichlet"));
      TS_ASSERT(!apriori4->isOfType("NoApriori"));

      vect = apriori4->getAllApriori(idset1);
      TS_ASSERT(vect.size() == std::size_t(3));
      TS_ASSERT(vect[0] == 1200.0);
      TS_ASSERT(vect[1] == 125.0);
      TS_ASSERT(vect[2] == 75.0);

      vect = apriori4->getConditioningApriori(idset1);
      TS_ASSERT(vect.size() == std::size_t(0));

      vect = apriori4->getAllApriori(idset2);
      TS_ASSERT(vect.size() == std::size_t(9));
      TS_ASSERT(vect[0] == 200.0);    // 0,0
      TS_ASSERT(vect[1] == 75.0);     // 1,0
      TS_ASSERT(vect[2] == 0.0);      // 2,0
      TS_ASSERT(vect[3] == 1000.0);   // 0,1
      TS_ASSERT(vect[4] == 0.0);      // 1,1
      TS_ASSERT(vect[5] == 75.0);     // 2,1
      TS_ASSERT(vect[6] == 0.0);      // 0,2
      TS_ASSERT(vect[7] == 50.0);     // 1,2
      TS_ASSERT(vect[8] == 0.0);      // 2,2

      vect = apriori4->getConditioningApriori(idset2);
      TS_ASSERT(vect.size() == std::size_t(0));

      vect = apriori4->getAllApriori(idset3);
      TS_ASSERT(vect.size() == std::size_t(9));
      TS_ASSERT(vect[0] == 0.0);      // 0,0
      TS_ASSERT(vect[1] == 75.0);     // 1,0
      TS_ASSERT(vect[2] == 0.0);      // 2,0
      TS_ASSERT(vect[3] == 200.0);    // 0,1
      TS_ASSERT(vect[4] == 50.0);     // 1,1
      TS_ASSERT(vect[5] == 75.0);     // 2,1
      TS_ASSERT(vect[6] == 1000.0);   // 0,2
      TS_ASSERT(vect[7] == 0.0);      // 1,2
      TS_ASSERT(vect[8] == 0.0);      // 2,2

      vect = apriori4->getConditioningApriori(idset3);
      TS_ASSERT(vect.size() == std::size_t(3));
      TS_ASSERT(vect[0] == 75.0);
      TS_ASSERT(vect[1] == 325.0);
      TS_ASSERT(vect[2] == 1000.0);


      apriori4->operator=(std::move(apriori));
      TS_ASSERT(apriori4->weight() == 2.0);
      apriori4->setWeight(1.0);
      TS_ASSERT(apriori4->weight() == 1.0);

      TS_ASSERT(apriori4->getType() == "Dirichlet");
      TS_ASSERT(apriori4->isOfType("Dirichlet"));
      TS_ASSERT(!apriori4->isOfType("NoApriori"));

      vect = apriori4->getAllApriori(idset1);
      TS_ASSERT(vect.size() == std::size_t(3));
      TS_ASSERT(vect[0] == 1200.0);
      TS_ASSERT(vect[1] == 125.0);
      TS_ASSERT(vect[2] == 75.0);

      vect = apriori4->getConditioningApriori(idset1);
      TS_ASSERT(vect.size() == std::size_t(0));

      vect = apriori4->getAllApriori(idset2);
      TS_ASSERT(vect.size() == std::size_t(9));
      TS_ASSERT(vect[0] == 200.0);    // 0,0
      TS_ASSERT(vect[1] == 75.0);     // 1,0
      TS_ASSERT(vect[2] == 0.0);      // 2,0
      TS_ASSERT(vect[3] == 1000.0);   // 0,1
      TS_ASSERT(vect[4] == 0.0);      // 1,1
      TS_ASSERT(vect[5] == 75.0);     // 2,1
      TS_ASSERT(vect[6] == 0.0);      // 0,2
      TS_ASSERT(vect[7] == 50.0);     // 1,2
      TS_ASSERT(vect[8] == 0.0);      // 2,2

      vect = apriori4->getConditioningApriori(idset2);
      TS_ASSERT(vect.size() == std::size_t(0));

      vect = apriori4->getAllApriori(idset3);
      TS_ASSERT(vect.size() == std::size_t(9));
      TS_ASSERT(vect[0] == 0.0);      // 0,0
      TS_ASSERT(vect[1] == 75.0);     // 1,0
      TS_ASSERT(vect[2] == 0.0);      // 2,0
      TS_ASSERT(vect[3] == 200.0);    // 0,1
      TS_ASSERT(vect[4] == 50.0);     // 1,1
      TS_ASSERT(vect[5] == 75.0);     // 2,1
      TS_ASSERT(vect[6] == 1000.0);   // 0,2
      TS_ASSERT(vect[7] == 0.0);      // 1,2
      TS_ASSERT(vect[8] == 0.0);      // 2,2

      vect = apriori4->getConditioningApriori(idset3);
      TS_ASSERT(vect.size() == std::size_t(3));
      TS_ASSERT(vect[0] == 75.0);
      TS_ASSERT(vect[1] == 325.0);
      TS_ASSERT(vect[2] == 1000.0);

      delete apriori4;
    }
  };


} /* namespace gum_tests */
