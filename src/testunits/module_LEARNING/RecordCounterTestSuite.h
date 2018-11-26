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
#include <agrum/learning/database/DBRowGeneratorParser.h>
#include <agrum/learning/scores_and_tests/recordCounter.h>

namespace gum_tests {

  class RecordCounterTestSuite : public CxxTest::TestSuite {
    public:
    void test_no_range__no_nodeId2col() {
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
          // translator.setName ( name );
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

      // create the record counter
      gum::learning::RecordCounter<> counter(parser);

      gum::learning::IdSet<> ids(0, std::vector< gum::NodeId >{2, 1}, true);
      std::vector< double >  counts = counter.counts(ids);

      TS_ASSERT(counts.size() == std::size_t(27));
      TS_ASSERT(counts[0] == double(200));    // A=0, C=0, B=0
      TS_ASSERT(counts[1] == double(75));     // A=1, C=0, B=0
      TS_ASSERT(counts[11] == double(75));    // A=2, C=0, B=1
      TS_ASSERT(counts[19] == double(50));    // A=1, C=0, B=2
      TS_ASSERT(counts[9] == double(1000));   // A=0, C=1, B=0
      gum::Set< std::size_t > xxx{0, 1, 11, 19, 9};
      for (std::size_t i = std::size_t(0); i < counts.size(); ++i) {
        if (!xxx.exists(i)) { TS_ASSERT(counts[i] == 0.0); }
      }

      gum::learning::IdSet<> ids2(2, std::vector< gum::NodeId >{0}, true);
      counts = counter.counts(ids2);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(1200));   // A=0, C=0
      TS_ASSERT(counts[3] == double(125));    // A=1, C=0
      TS_ASSERT(counts[6] == double(75));     // A=2, C=0

      gum::learning::IdSet<> ids3(0, std::vector< gum::NodeId >(), true);
      counts = counter.counts(ids3);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(1200));   // A=0
      TS_ASSERT(counts[1] == double(125));    // A=1
      TS_ASSERT(counts[2] == double(75));     // A=2

      gum::learning::IdSet<> ids4(2, std::vector< gum::NodeId >(), true);
      counts = counter.counts(ids4);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(1400));   // C=0
      TS_ASSERT(counts[1] == double(0));      // C=1
      TS_ASSERT(counts[2] == double(0));      // C=2

      gum::learning::IdSet<> ids5(0, std::vector< gum::NodeId >{1}, true);
      counts = counter.counts(ids5);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(200));    // A=0, B=0
      TS_ASSERT(counts[1] == double(75));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));      // A=2, B=0
      TS_ASSERT(counts[3] == double(1000));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));      // A=1, B=1
      TS_ASSERT(counts[5] == double(75));     // A=2, B=1
      TS_ASSERT(counts[6] == double(0));      // A=0, B=2
      TS_ASSERT(counts[7] == double(50));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));      // A=2, B=2

      gum::learning::IdSet<> ids6(3, std::vector< gum::NodeId >(), true);
      counts = counter.counts(ids6);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(75));     // D=0
      TS_ASSERT(counts[1] == double(325));    // D=1
      TS_ASSERT(counts[2] == double(1000));   // D=2

      counts = counter.counts(ids6);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(75));     // D=0
      TS_ASSERT(counts[1] == double(325));    // D=1
      TS_ASSERT(counts[2] == double(1000));   // D=2

      counter.clear();
      counts = counter.counts(ids6);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(75));     // D=0
      TS_ASSERT(counts[1] == double(325));    // D=1
      TS_ASSERT(counts[2] == double(1000));   // D=2

      gum::learning::IdSet<> ids7(0, std::vector< gum::NodeId >{1}, true);
      counts = counter.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(200));    // A=0, B=0
      TS_ASSERT(counts[1] == double(75));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));      // A=2, B=0
      TS_ASSERT(counts[3] == double(1000));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));      // A=1, B=1
      TS_ASSERT(counts[5] == double(75));     // A=2, B=1
      TS_ASSERT(counts[6] == double(0));      // A=0, B=2
      TS_ASSERT(counts[7] == double(50));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));      // A=2, B=2

      std::vector< std::pair< std::size_t, std::size_t > > new_ranges{
        std::pair< std::size_t, std::size_t >(500, 600),
        std::pair< std::size_t, std::size_t >(1050, 1125),
        std::pair< std::size_t, std::size_t >(100, 150)};
      counter.setRanges(new_ranges);

      counts = counter.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2

      gum::learning::RecordCounter<> counter2(counter);
      counts = counter2.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2

      gum::learning::RecordCounter<> counter3(std::move(counter2));
      counts = counter3.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2

      gum::learning::RecordCounter<>* counter4 = counter.clone();
      counts = counter4->counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2
      counter.clearRanges();
      counter3 = counter;
      counts = counter3.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(200));    // A=0, B=0
      TS_ASSERT(counts[1] == double(75));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));      // A=2, B=0
      TS_ASSERT(counts[3] == double(1000));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));      // A=1, B=1
      TS_ASSERT(counts[5] == double(75));     // A=2, B=1
      TS_ASSERT(counts[6] == double(0));      // A=0, B=2
      TS_ASSERT(counts[7] == double(50));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));      // A=2, B=2

      counter3 = std::move(*counter4);
      delete counter4;
      counts = counter3.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2
    }


    void test_no_range__has_nodeId2col() {
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
          // translator.setName ( name );
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

      // create the record counter
      gum::Bijection< gum::NodeId, std::size_t > nodeId2columns;
      nodeId2columns.insert(gum::NodeId(0), std::size_t(2));
      nodeId2columns.insert(gum::NodeId(1), std::size_t(4));
      nodeId2columns.insert(gum::NodeId(2), std::size_t(5));
      nodeId2columns.insert(gum::NodeId(3), std::size_t(1));
      nodeId2columns.insert(gum::NodeId(4), std::size_t(3));
      nodeId2columns.insert(gum::NodeId(5), std::size_t(0));
      std::vector< std::pair< std::size_t, std::size_t > > ranges{
        std::pair< std::size_t, std::size_t >(std::size_t(0), database.nbRows())};
      gum::learning::RecordCounter<> counter(parser, ranges, nodeId2columns);

      gum::learning::IdSet<> ids(5, std::vector< gum::NodeId >{0, 3}, true);
      std::vector< double >  counts = counter.counts(ids);
      TS_ASSERT(counts.size() == std::size_t(27));
      TS_ASSERT(counts[0] == double(200));    // A=0, C=0, B=0
      TS_ASSERT(counts[1] == double(75));     // A=1, C=0, B=0
      TS_ASSERT(counts[11] == double(75));    // A=2, C=0, B=1
      TS_ASSERT(counts[19] == double(50));    // A=1, C=0, B=2
      TS_ASSERT(counts[9] == double(1000));   // A=0, C=1, B=0
      gum::Set< std::size_t > xxx{0, 1, 11, 19, 9};
      for (std::size_t i = std::size_t(0); i < counts.size(); ++i) {
        if (!xxx.exists(i)) { TS_ASSERT(counts[i] == 0.0); }
      }

      gum::learning::IdSet<> ids1(5, 0, 3, std::vector< gum::NodeId >(), true);
      counts = counter.counts(ids1);
      TS_ASSERT(counts.size() == std::size_t(27));
      TS_ASSERT(counts[0] == double(200));    // A=0, C=0, B=0
      TS_ASSERT(counts[1] == double(75));     // A=1, C=0, B=0
      TS_ASSERT(counts[11] == double(75));    // A=2, C=0, B=1
      TS_ASSERT(counts[19] == double(50));    // A=1, C=0, B=2
      TS_ASSERT(counts[9] == double(1000));   // A=0, C=1, B=0
      for (std::size_t i = std::size_t(0); i < counts.size(); ++i) {
        if (!xxx.exists(i)) { TS_ASSERT(counts[i] == 0.0); }
      }

      gum::learning::IdSet<> ids2(0, std::vector< gum::NodeId >{5}, true);
      counts = counter.counts(ids2);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(1200));   // A=0, C=0
      TS_ASSERT(counts[3] == double(125));    // A=1, C=0
      TS_ASSERT(counts[6] == double(75));     // A=2, C=0

      gum::learning::IdSet<> ids3(5, std::vector< gum::NodeId >(), true);
      counts = counter.counts(ids3);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(1200));   // A=0
      TS_ASSERT(counts[1] == double(125));    // A=1
      TS_ASSERT(counts[2] == double(75));     // A=2

      gum::learning::IdSet<> ids4(0, std::vector< gum::NodeId >(), true);
      counts = counter.counts(ids4);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(1400));   // C=0
      TS_ASSERT(counts[1] == double(0));      // C=1
      TS_ASSERT(counts[2] == double(0));      // C=2

      gum::learning::IdSet<> ids5(5, 3, std::vector< gum::NodeId >(), true);
      counts = counter.counts(ids5);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(200));    // A=0, B=0
      TS_ASSERT(counts[1] == double(75));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));      // A=2, B=0
      TS_ASSERT(counts[3] == double(1000));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));      // A=1, B=1
      TS_ASSERT(counts[5] == double(75));     // A=2, B=1
      TS_ASSERT(counts[6] == double(0));      // A=0, B=2
      TS_ASSERT(counts[7] == double(50));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));      // A=2, B=2

      gum::learning::IdSet<> ids6(4, std::vector< gum::NodeId >(), true);
      counts = counter.counts(ids6);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(75));     // D=0
      TS_ASSERT(counts[1] == double(325));    // D=1
      TS_ASSERT(counts[2] == double(1000));   // D=2

      counts = counter.counts(ids6);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(75));     // D=0
      TS_ASSERT(counts[1] == double(325));    // D=1
      TS_ASSERT(counts[2] == double(1000));   // D=2

      counter.clear();
      counts = counter.counts(ids6);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(75));     // D=0
      TS_ASSERT(counts[1] == double(325));    // D=1
      TS_ASSERT(counts[2] == double(1000));   // D=2

      gum::learning::IdSet<> ids7(5, 3, std::vector< gum::NodeId >(), true);
      counts = counter.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(200));    // A=0, B=0
      TS_ASSERT(counts[1] == double(75));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));      // A=2, B=0
      TS_ASSERT(counts[3] == double(1000));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));      // A=1, B=1
      TS_ASSERT(counts[5] == double(75));     // A=2, B=1
      TS_ASSERT(counts[6] == double(0));      // A=0, B=2
      TS_ASSERT(counts[7] == double(50));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));      // A=2, B=2

      std::vector< std::pair< std::size_t, std::size_t > > new_ranges{
        std::pair< std::size_t, std::size_t >(500, 600),
        std::pair< std::size_t, std::size_t >(1050, 1125),
        std::pair< std::size_t, std::size_t >(100, 150)};
      counter.setRanges(new_ranges);

      counts = counter.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2

      gum::learning::RecordCounter<> counter2(counter);
      TS_ASSERT(counter2.nbThreads() == gum::getMaxNumberOfThreads());
      counter2.setMaxNbThreads(std::size_t(2));
      if (gum::isOMP()) {
        TS_ASSERT(counter2.nbThreads() == std::size_t(2));
      } else {
        TS_ASSERT(counter2.nbThreads() == std::size_t(1));
      }
      counts = counter2.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2

      gum::learning::RecordCounter<> counter3(std::move(counter2));
      counts = counter3.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2

      gum::learning::RecordCounter<>* counter4 = counter.clone();
      counts = counter4->counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2
      counter.clearRanges();
      counter3 = counter;
      TS_ASSERT(counter3.nbThreads() == gum::getMaxNumberOfThreads());
      counter3.setMinNbRowsPerThread(std::size_t(10000));
      counts = counter3.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(200));    // A=0, B=0
      TS_ASSERT(counts[1] == double(75));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));      // A=2, B=0
      TS_ASSERT(counts[3] == double(1000));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));      // A=1, B=1
      TS_ASSERT(counts[5] == double(75));     // A=2, B=1
      TS_ASSERT(counts[6] == double(0));      // A=0, B=2
      TS_ASSERT(counts[7] == double(50));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));      // A=2, B=2

      counter3 = std::move(*counter4);
      delete counter4;
      counts = counter3.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2
    }


    void test_partial_nodes() {
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
          // translator.setName ( name );
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

      // create the record counter
      gum::Bijection< gum::NodeId, std::size_t > nodeId2columns;
      nodeId2columns.insert(gum::NodeId(0), std::size_t(2));
      nodeId2columns.insert(gum::NodeId(1), std::size_t(4));
      nodeId2columns.insert(gum::NodeId(2), std::size_t(5));
      nodeId2columns.insert(gum::NodeId(3), std::size_t(1));
      nodeId2columns.insert(gum::NodeId(4), std::size_t(0));
      std::vector< std::pair< std::size_t, std::size_t > > ranges{
        std::pair< std::size_t, std::size_t >(std::size_t(0), database.nbRows())};
      gum::learning::RecordCounter<> counter(parser, ranges, nodeId2columns);

      gum::learning::IdSet<> ids(4, std::vector< gum::NodeId >{0, 3}, true);
      std::vector< double >  counts = counter.counts(ids);
      TS_ASSERT(counts.size() == std::size_t(27));
      TS_ASSERT(counts[0] == double(200));    // A=0, C=0, B=0
      TS_ASSERT(counts[1] == double(75));     // A=1, C=0, B=0
      TS_ASSERT(counts[11] == double(75));    // A=2, C=0, B=1
      TS_ASSERT(counts[19] == double(50));    // A=1, C=0, B=2
      TS_ASSERT(counts[9] == double(1000));   // A=0, C=1, B=0
      gum::Set< std::size_t > xxx{0, 1, 11, 19, 9};
      for (std::size_t i = std::size_t(0); i < counts.size(); ++i) {
        if (!xxx.exists(i)) { TS_ASSERT(counts[i] == 0.0); }
      }

      gum::learning::IdSet<> ids1(4, 0, 3, std::vector< gum::NodeId >(), true);
      counts = counter.counts(ids1);
      TS_ASSERT(counts.size() == std::size_t(27));
      TS_ASSERT(counts[0] == double(200));    // A=0, C=0, B=0
      TS_ASSERT(counts[1] == double(75));     // A=1, C=0, B=0
      TS_ASSERT(counts[11] == double(75));    // A=2, C=0, B=1
      TS_ASSERT(counts[19] == double(50));    // A=1, C=0, B=2
      TS_ASSERT(counts[9] == double(1000));   // A=0, C=1, B=0
      for (std::size_t i = std::size_t(0); i < counts.size(); ++i) {
        if (!xxx.exists(i)) { TS_ASSERT(counts[i] == 0.0); }
      }

      gum::learning::IdSet<> ids4(5, std::vector< gum::NodeId >(), true);
      TS_ASSERT_THROWS(counts = counter.counts(ids4), gum::NotFound);
    }


    void test_has_range_no_nodeId2col() {
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
          // translator.setName ( name );
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

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000},
                                                                  {1050, 1400}};

      // create the record counter
      gum::learning::RecordCounter<> counter(parser, ranges);

      gum::learning::IdSet<> ids(0, std::vector< gum::NodeId >{2, 1}, true);
      std::vector< double >  counts = counter.counts(ids);

      TS_ASSERT(counts.size() == std::size_t(27));
      TS_ASSERT(counts[9] == double(200));   // A=0, C=1, B=0
      TS_ASSERT(counts[11] == double(75));   // A=2, C=0, B=1
      TS_ASSERT(counts[0] == double(200));   // A=0, C=0, B=0
      TS_ASSERT(counts[1] == double(75));    // A=1, C=0, B=0
      gum::Set< std::size_t > xxx{0, 1, 11, 9};
      for (std::size_t i = std::size_t(0); i < counts.size(); ++i) {
        if (!xxx.exists(i)) { TS_ASSERT(counts[i] == 0.0); }
      }

      gum::learning::IdSet<> ids2(2, std::vector< gum::NodeId >{0}, true);
      counts = counter.counts(ids2);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(400));   // A=0, C=0
      TS_ASSERT(counts[3] == double(75));    // A=1, C=0
      TS_ASSERT(counts[6] == double(75));    // A=2, C=0

      gum::learning::IdSet<> ids3(0, std::vector< gum::NodeId >(), true);
      counts = counter.counts(ids3);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(400));   // A=0
      TS_ASSERT(counts[1] == double(75));    // A=1
      TS_ASSERT(counts[2] == double(75));    // A=2

      gum::learning::IdSet<> ids4(2, std::vector< gum::NodeId >(), true);
      counts = counter.counts(ids4);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(550));   // C=0
      TS_ASSERT(counts[1] == double(0));     // C=1
      TS_ASSERT(counts[2] == double(0));     // C=2

      gum::learning::IdSet<> ids5(0, std::vector< gum::NodeId >{1}, true);
      counts = counter.counts(ids5);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(200));   // A=0, B=0
      TS_ASSERT(counts[1] == double(75));    // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(200));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2

      gum::learning::IdSet<> ids6(3, std::vector< gum::NodeId >(), true);
      counts = counter.counts(ids6);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(75));    // D=0
      TS_ASSERT(counts[1] == double(275));   // D=1
      TS_ASSERT(counts[2] == double(200));   // D=2

      counts = counter.counts(ids6);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(75));    // D=0
      TS_ASSERT(counts[1] == double(275));   // D=1
      TS_ASSERT(counts[2] == double(200));   // D=2

      counter.clear();
      counts = counter.counts(ids6);
      TS_ASSERT(counts.size() == std::size_t(3));
      TS_ASSERT(counts[0] == double(75));    // D=0
      TS_ASSERT(counts[1] == double(275));   // D=1
      TS_ASSERT(counts[2] == double(200));   // D=2

      gum::learning::IdSet<> ids7(0, std::vector< gum::NodeId >{1}, true);
      counts = counter.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(200));   // A=0, B=0
      TS_ASSERT(counts[1] == double(75));    // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(200));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2

      std::vector< std::pair< std::size_t, std::size_t > > new_ranges{
        std::pair< std::size_t, std::size_t >(500, 600),
        std::pair< std::size_t, std::size_t >(1050, 1125),
        std::pair< std::size_t, std::size_t >(100, 150)};
      counter.setRanges(new_ranges);

      counts = counter.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2

      gum::learning::RecordCounter<> counter2(counter);
      counts = counter2.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2

      gum::learning::RecordCounter<> counter3(std::move(counter2));
      counts = counter3.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2

      gum::learning::RecordCounter<>* counter4 = counter.clone();
      counts = counter4->counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2
      counter.clearRanges();
      counter3 = counter;
      counts = counter3.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(200));    // A=0, B=0
      TS_ASSERT(counts[1] == double(75));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));      // A=2, B=0
      TS_ASSERT(counts[3] == double(1000));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));      // A=1, B=1
      TS_ASSERT(counts[5] == double(75));     // A=2, B=1
      TS_ASSERT(counts[6] == double(0));      // A=0, B=2
      TS_ASSERT(counts[7] == double(50));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));      // A=2, B=2

      counter3 = std::move(*counter4);
      delete counter4;
      counts = counter3.counts(ids7);
      TS_ASSERT(counts.size() == std::size_t(9));
      TS_ASSERT(counts[0] == double(0));     // A=0, B=0
      TS_ASSERT(counts[1] == double(0));     // A=1, B=0
      TS_ASSERT(counts[2] == double(0));     // A=2, B=0
      TS_ASSERT(counts[3] == double(150));   // A=0, B=1
      TS_ASSERT(counts[4] == double(0));     // A=1, B=1
      TS_ASSERT(counts[5] == double(75));    // A=2, B=1
      TS_ASSERT(counts[6] == double(0));     // A=0, B=2
      TS_ASSERT(counts[7] == double(0));     // A=1, B=2
      TS_ASSERT(counts[8] == double(0));     // A=2, B=2
    }


    void test_multicore() {
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
          // translator.setName ( name );
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

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000},
                                                                  {1050, 1400}};

      for (std::size_t i = std::size_t(1); i < std::size_t(24); ++i) {
        // create the record counter
        gum::learning::RecordCounter<> counter(parser, ranges);
        counter.setMaxNbThreads(i);

        gum::learning::IdSet<> ids(0, std::vector< gum::NodeId >{2, 1}, true);
        std::vector< double >  counts = counter.counts(ids);

        TS_ASSERT(counts.size() == std::size_t(27));
        TS_ASSERT(counts[9] == double(200));   // A=0, C=1, B=0
        TS_ASSERT(counts[11] == double(75));   // A=2, C=0, B=1
        TS_ASSERT(counts[0] == double(200));   // A=0, C=0, B=0
        TS_ASSERT(counts[1] == double(75));    // A=1, C=0, B=0
        gum::Set< std::size_t > xxx{0, 1, 11, 9};
        for (std::size_t i = std::size_t(0); i < counts.size(); ++i) {
          if (!xxx.exists(i)) { TS_ASSERT(counts[i] == 0.0); }
        }

        gum::learning::IdSet<> ids2(2, std::vector< gum::NodeId >{0}, true);
        counts = counter.counts(ids2);
        TS_ASSERT(counts.size() == std::size_t(9));
        TS_ASSERT(counts[0] == double(400));   // A=0, C=0
        TS_ASSERT(counts[3] == double(75));    // A=1, C=0
        TS_ASSERT(counts[6] == double(75));    // A=2, C=0

        gum::learning::IdSet<> ids3(0, std::vector< gum::NodeId >(), true);
        counts = counter.counts(ids3);
        TS_ASSERT(counts.size() == std::size_t(3));
        TS_ASSERT(counts[0] == double(400));   // A=0
        TS_ASSERT(counts[1] == double(75));    // A=1
        TS_ASSERT(counts[2] == double(75));    // A=2

        gum::learning::IdSet<> ids4(2, std::vector< gum::NodeId >(), true);
        counts = counter.counts(ids4);
        TS_ASSERT(counts.size() == std::size_t(3));
        TS_ASSERT(counts[0] == double(550));   // C=0
        TS_ASSERT(counts[1] == double(0));     // C=1
        TS_ASSERT(counts[2] == double(0));     // C=2

        gum::learning::IdSet<> ids5(0, std::vector< gum::NodeId >{1}, true);
        counts = counter.counts(ids5);
        TS_ASSERT(counts.size() == std::size_t(9));
        TS_ASSERT(counts[0] == double(200));   // A=0, B=0
        TS_ASSERT(counts[1] == double(75));    // A=1, B=0
        TS_ASSERT(counts[2] == double(0));     // A=2, B=0
        TS_ASSERT(counts[3] == double(200));   // A=0, B=1
        TS_ASSERT(counts[4] == double(0));     // A=1, B=1
        TS_ASSERT(counts[5] == double(75));    // A=2, B=1
        TS_ASSERT(counts[6] == double(0));     // A=0, B=2
        TS_ASSERT(counts[7] == double(0));     // A=1, B=2
        TS_ASSERT(counts[8] == double(0));     // A=2, B=2

        gum::learning::IdSet<> ids6(3, std::vector< gum::NodeId >(), true);
        counts = counter.counts(ids6);
        TS_ASSERT(counts.size() == std::size_t(3));
        TS_ASSERT(counts[0] == double(75));    // D=0
        TS_ASSERT(counts[1] == double(275));   // D=1
        TS_ASSERT(counts[2] == double(200));   // D=2

        counts = counter.counts(ids6);
        TS_ASSERT(counts.size() == std::size_t(3));
        TS_ASSERT(counts[0] == double(75));    // D=0
        TS_ASSERT(counts[1] == double(275));   // D=1
        TS_ASSERT(counts[2] == double(200));   // D=2

        counter.clear();
        counts = counter.counts(ids6);
        TS_ASSERT(counts.size() == std::size_t(3));
        TS_ASSERT(counts[0] == double(75));    // D=0
        TS_ASSERT(counts[1] == double(275));   // D=1
        TS_ASSERT(counts[2] == double(200));   // D=2

        gum::learning::IdSet<> ids7(0, std::vector< gum::NodeId >{1}, true);
        counts = counter.counts(ids7);
        TS_ASSERT(counts.size() == std::size_t(9));
        TS_ASSERT(counts[0] == double(200));   // A=0, B=0
        TS_ASSERT(counts[1] == double(75));    // A=1, B=0
        TS_ASSERT(counts[2] == double(0));     // A=2, B=0
        TS_ASSERT(counts[3] == double(200));   // A=0, B=1
        TS_ASSERT(counts[4] == double(0));     // A=1, B=1
        TS_ASSERT(counts[5] == double(75));    // A=2, B=1
        TS_ASSERT(counts[6] == double(0));     // A=0, B=2
        TS_ASSERT(counts[7] == double(0));     // A=1, B=2
        TS_ASSERT(counts[8] == double(0));     // A=2, B=2

        std::vector< std::pair< std::size_t, std::size_t > > new_ranges{
          std::pair< std::size_t, std::size_t >(500, 600),
          std::pair< std::size_t, std::size_t >(1050, 1125),
          std::pair< std::size_t, std::size_t >(100, 150)};
        counter.setRanges(new_ranges);

        counts = counter.counts(ids7);
        TS_ASSERT(counts.size() == std::size_t(9));
        TS_ASSERT(counts[0] == double(0));     // A=0, B=0
        TS_ASSERT(counts[1] == double(0));     // A=1, B=0
        TS_ASSERT(counts[2] == double(0));     // A=2, B=0
        TS_ASSERT(counts[3] == double(150));   // A=0, B=1
        TS_ASSERT(counts[4] == double(0));     // A=1, B=1
        TS_ASSERT(counts[5] == double(75));    // A=2, B=1
        TS_ASSERT(counts[6] == double(0));     // A=0, B=2
        TS_ASSERT(counts[7] == double(0));     // A=1, B=2
        TS_ASSERT(counts[8] == double(0));     // A=2, B=2

        gum::learning::RecordCounter<> counter2(counter);
        counts = counter2.counts(ids7);
        TS_ASSERT(counts.size() == std::size_t(9));
        TS_ASSERT(counts[0] == double(0));     // A=0, B=0
        TS_ASSERT(counts[1] == double(0));     // A=1, B=0
        TS_ASSERT(counts[2] == double(0));     // A=2, B=0
        TS_ASSERT(counts[3] == double(150));   // A=0, B=1
        TS_ASSERT(counts[4] == double(0));     // A=1, B=1
        TS_ASSERT(counts[5] == double(75));    // A=2, B=1
        TS_ASSERT(counts[6] == double(0));     // A=0, B=2
        TS_ASSERT(counts[7] == double(0));     // A=1, B=2
        TS_ASSERT(counts[8] == double(0));     // A=2, B=2

        gum::learning::RecordCounter<> counter3(std::move(counter2));
        counts = counter3.counts(ids7);
        TS_ASSERT(counts.size() == std::size_t(9));
        TS_ASSERT(counts[0] == double(0));     // A=0, B=0
        TS_ASSERT(counts[1] == double(0));     // A=1, B=0
        TS_ASSERT(counts[2] == double(0));     // A=2, B=0
        TS_ASSERT(counts[3] == double(150));   // A=0, B=1
        TS_ASSERT(counts[4] == double(0));     // A=1, B=1
        TS_ASSERT(counts[5] == double(75));    // A=2, B=1
        TS_ASSERT(counts[6] == double(0));     // A=0, B=2
        TS_ASSERT(counts[7] == double(0));     // A=1, B=2
        TS_ASSERT(counts[8] == double(0));     // A=2, B=2

        gum::learning::RecordCounter<>* counter4 = counter.clone();
        counts = counter4->counts(ids7);
        TS_ASSERT(counts.size() == std::size_t(9));
        TS_ASSERT(counts[0] == double(0));     // A=0, B=0
        TS_ASSERT(counts[1] == double(0));     // A=1, B=0
        TS_ASSERT(counts[2] == double(0));     // A=2, B=0
        TS_ASSERT(counts[3] == double(150));   // A=0, B=1
        TS_ASSERT(counts[4] == double(0));     // A=1, B=1
        TS_ASSERT(counts[5] == double(75));    // A=2, B=1
        TS_ASSERT(counts[6] == double(0));     // A=0, B=2
        TS_ASSERT(counts[7] == double(0));     // A=1, B=2
        TS_ASSERT(counts[8] == double(0));     // A=2, B=2
        counter.clearRanges();
        counter3 = counter;
        counts = counter3.counts(ids7);
        TS_ASSERT(counts.size() == std::size_t(9));
        TS_ASSERT(counts[0] == double(200));    // A=0, B=0
        TS_ASSERT(counts[1] == double(75));     // A=1, B=0
        TS_ASSERT(counts[2] == double(0));      // A=2, B=0
        TS_ASSERT(counts[3] == double(1000));   // A=0, B=1
        TS_ASSERT(counts[4] == double(0));      // A=1, B=1
        TS_ASSERT(counts[5] == double(75));     // A=2, B=1
        TS_ASSERT(counts[6] == double(0));      // A=0, B=2
        TS_ASSERT(counts[7] == double(50));     // A=1, B=2
        TS_ASSERT(counts[8] == double(0));      // A=2, B=2

        counter3 = std::move(*counter4);
        delete counter4;
        counts = counter3.counts(ids7);
        TS_ASSERT(counts.size() == std::size_t(9));
        TS_ASSERT(counts[0] == double(0));     // A=0, B=0
        TS_ASSERT(counts[1] == double(0));     // A=1, B=0
        TS_ASSERT(counts[2] == double(0));     // A=2, B=0
        TS_ASSERT(counts[3] == double(150));   // A=0, B=1
        TS_ASSERT(counts[4] == double(0));     // A=1, B=1
        TS_ASSERT(counts[5] == double(75));    // A=2, B=1
        TS_ASSERT(counts[6] == double(0));     // A=0, B=2
        TS_ASSERT(counts[7] == double(0));     // A=1, B=2
        TS_ASSERT(counts[8] == double(0));     // A=2, B=2
      }
    }
  };


} /* namespace gum_tests */
